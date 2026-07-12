#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {

constexpr double kSheetWidthIn = 48.0;
constexpr double kSheetLengthIn = 96.0;
constexpr double kKerfIn = 0.125;
constexpr const char* kDefaultInputFile = "cut_list.csv";
constexpr const char* kDefaultOutputFile = "cut_sequence.csv";
constexpr const char* kDefaultLayoutSvgFile = "cut_layout.svg";

struct CutRequest {
	std::string id;
	double len_in = 0.0;
	double width_in = 0.0;
};

struct Piece {
	std::string id;
	int copy_index = 0;
	double len_in = 0.0;
	double width_in = 0.0;
};

struct Placement {
	Piece piece;
	double rip_start_in = 0.0;
	double strip_offset_in = 0.0;
};

struct Lane {
	double rip_start_in = 0.0;
	double width_in = 0.0;
	double used_len_in = 0.0;
	std::vector<Placement> placements;
};

struct Strip {
	double len_in = 0.0;
	double used_width_in = 0.0;
	std::vector<Lane> lanes;
};

struct PlanStep {
	int seq = 0;
	int strip_no = 0;
	double strip_start_in = 0.0;
	double strip_len_in = 0.0;
	double rip_start_in = 0.0;
	double piece_start_in = 0.0;
	std::string id;
	int piece_index = 0;
	double len_in = 0.0;
	double width_in = 0.0;
};

std::string escapeXml(const std::string& s) {
	std::string out;
	out.reserve(s.size());
	for (char c : s) {
		switch (c) {
			case '&': out += "&amp;"; break;
			case '<': out += "&lt;"; break;
			case '>': out += "&gt;"; break;
			case '"': out += "&quot;"; break;
			case '\'': out += "&apos;"; break;
			default: out.push_back(c); break;
		}
	}
	return out;
}

std::string trim(const std::string& s) {
	size_t start = 0;
	while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
		++start;
	}
	size_t end = s.size();
	while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
		--end;
	}
	return s.substr(start, end - start);
}

std::string toLower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
	return s;
}

std::vector<std::string> splitLine(const std::string& line, char delimiter) {
	std::vector<std::string> out;
	std::stringstream ss(line);
	std::string token;
	while (std::getline(ss, token, delimiter)) {
		out.push_back(trim(token));
	}
	return out;
}

char detectDelimiter(const std::string& header) {
	const std::vector<char> candidates = {',', '|', '\t'};
	char best = ',';
	size_t best_count = 0;

	for (char c : candidates) {
		size_t count = static_cast<size_t>(std::count(header.begin(), header.end(), c));
		if (count > best_count) {
			best_count = count;
			best = c;
		}
	}
	return best;
}

bool parseDouble(const std::string& s, double& out) {
	try {
		size_t pos = 0;
		out = std::stod(s, &pos);
		return pos == s.size();
	} catch (...) {
		return false;
	}
}

std::vector<CutRequest> readCsv(const std::string& path) {
	std::ifstream in(path);
	if (!in) {
		throw std::runtime_error("Could not open input file: " + path);
	}

	std::string header;
	if (!std::getline(in, header)) {
		throw std::runtime_error("Input CSV appears to be empty.");
	}

	const char delimiter = detectDelimiter(header);
	std::vector<std::string> cols = splitLine(header, delimiter);
	for (std::string& c : cols) {
		c = toLower(c);
	}

	auto idxOf = [&](const std::string& name) -> int {
		for (size_t i = 0; i < cols.size(); ++i) {
			if (cols[i] == name) {
				return static_cast<int>(i);
			}
		}
		return -1;
	};

	const int id_idx = idxOf("id");
	const int len_idx = idxOf("len_in");
	const int width_idx = idxOf("width_in");

	if (id_idx < 0 || len_idx < 0 || width_idx < 0) {
		throw std::runtime_error("CSV header must include: id, len_in, width_in");
	}

	std::vector<CutRequest> requests;
	std::unordered_set<std::string> seen_ids;
	std::string line;
	int line_no = 1;

	while (std::getline(in, line)) {
		++line_no;
		line = trim(line);
		if (line.empty()) {
			continue;
		}

		std::vector<std::string> vals = splitLine(line, delimiter);
		const int max_idx = std::max({id_idx, len_idx, width_idx});
		if (static_cast<int>(vals.size()) <= max_idx) {
			throw std::runtime_error("CSV parse error at line " + std::to_string(line_no) + ": not enough columns");
		}

		CutRequest req;
		req.id = vals[id_idx];
		if (req.id.empty()) {
			throw std::runtime_error("CSV parse error at line " + std::to_string(line_no) + ": id is empty");
		}
		if (!seen_ids.insert(req.id).second) {
			throw std::runtime_error("CSV parse error at line " + std::to_string(line_no) + ": duplicate id '" + req.id + "'");
		}

		if (!parseDouble(vals[len_idx], req.len_in) || req.len_in <= 0.0) {
			throw std::runtime_error("CSV parse error at line " + std::to_string(line_no) + ": invalid len_in");
		}
		if (!parseDouble(vals[width_idx], req.width_in) || req.width_in <= 0.0) {
			throw std::runtime_error("CSV parse error at line " + std::to_string(line_no) + ": invalid width_in");
		}

		requests.push_back(req);
	}

	if (requests.empty()) {
		throw std::runtime_error("No cut rows found in input CSV.");
	}

	return requests;
}

std::vector<CutRequest> loadRequests(const std::string& input_path) {
	const std::string ext = toLower(std::filesystem::path(input_path).extension().string());
	if (ext == ".csv") {
		return readCsv(input_path);
	}
	throw std::runtime_error("Unsupported input file extension. Use .csv only");
}

std::vector<Piece> expandPieces(const std::vector<CutRequest>& requests) {
	std::vector<Piece> pieces;
	for (size_t row_index = 0; row_index < requests.size(); ++row_index) {
		const CutRequest& r = requests[row_index];
		if (r.width_in > kSheetWidthIn) {
			throw std::runtime_error("Piece " + r.id + " has width_in > 48.0 and cannot fit on sheet.");
		}
		if (r.len_in > kSheetLengthIn) {
			throw std::runtime_error("Piece " + r.id + " has len_in > 96.0 and cannot fit on sheet.");
		}

		pieces.push_back(Piece{r.id, static_cast<int>(row_index + 1), r.len_in, r.width_in});
	}
	return pieces;
}

std::vector<Strip> optimizeStrips(std::vector<Piece> pieces) {
	// Sort bigger lengths first to reduce total consumed length.
	std::sort(pieces.begin(), pieces.end(), [](const Piece& a, const Piece& b) {
		if (a.len_in != b.len_in) {
			return a.len_in > b.len_in;
		}
		return a.width_in > b.width_in;
	});

	std::vector<Strip> strips;
	for (const Piece& p : pieces) {
		struct BestFit {
			int strip_idx = -1;
			int lane_idx = -1;
			bool use_new_lane = false;
			double delta_len = std::numeric_limits<double>::infinity();
			double width_slack = std::numeric_limits<double>::infinity();
			double rem_width = std::numeric_limits<double>::infinity();
			int reuse_rank = 1;
		};

		BestFit best;

		auto isBetter = [](const BestFit& cand, const BestFit& curr) {
			if (cand.delta_len < curr.delta_len - 1e-9) {
				return true;
			}
			if (std::abs(cand.delta_len - curr.delta_len) > 1e-9) {
				return false;
			}
			if (cand.reuse_rank != curr.reuse_rank) {
				return cand.reuse_rank < curr.reuse_rank;
			}
			if (cand.width_slack < curr.width_slack - 1e-9) {
				return true;
			}
			if (std::abs(cand.width_slack - curr.width_slack) > 1e-9) {
				return false;
			}
			return cand.rem_width < curr.rem_width - 1e-9;
		};

		for (int i = 0; i < static_cast<int>(strips.size()); ++i) {
			Strip& s = strips[i];

			// Try placing in an existing lane first (reuses internal leftovers).
			for (int j = 0; j < static_cast<int>(s.lanes.size()); ++j) {
				const Lane& lane = s.lanes[j];
				if (p.width_in > lane.width_in + 1e-9) {
					continue;
				}

				BestFit cand;
				cand.strip_idx = i;
				cand.lane_idx = j;
				cand.use_new_lane = false;
				cand.reuse_rank = 0;
				cand.width_slack = lane.width_in - p.width_in;
				cand.rem_width = kSheetWidthIn - s.used_width_in;
				const double lane_new_len = lane.used_len_in + kKerfIn + p.len_in;
				cand.delta_len = std::max(s.len_in, lane_new_len) - s.len_in;

				if (isBetter(cand, best)) {
					best = cand;
				}
			}

			// Optionally place as a new lane in this strip.
			const double lane_width_consumption = p.width_in + (s.lanes.empty() ? 0.0 : kKerfIn);
			if (s.used_width_in + lane_width_consumption <= kSheetWidthIn + 1e-9) {
				BestFit cand;
				cand.strip_idx = i;
				cand.lane_idx = -1;
				cand.use_new_lane = true;
				cand.reuse_rank = 1;
				cand.width_slack = 0.0;
				cand.rem_width = kSheetWidthIn - (s.used_width_in + lane_width_consumption);
				cand.delta_len = std::max(s.len_in, p.len_in) - s.len_in;

				if (isBetter(cand, best)) {
					best = cand;
				}
			}
		}

		if (best.strip_idx < 0) {
			Strip s;
			s.len_in = p.len_in;
			s.used_width_in = p.width_in;

			Lane lane;
			lane.rip_start_in = 0.0;
			lane.width_in = p.width_in;
			lane.used_len_in = p.len_in;
			lane.placements.push_back(Placement{p, lane.rip_start_in, 0.0});
			s.lanes.push_back(std::move(lane));

			strips.push_back(s);
		} else {
			Strip& s = strips[best.strip_idx];
			if (best.use_new_lane) {
				Lane lane;
				lane.rip_start_in = s.used_width_in + (s.lanes.empty() ? 0.0 : kKerfIn);
				lane.width_in = p.width_in;
				lane.used_len_in = p.len_in;
				lane.placements.push_back(Placement{p, lane.rip_start_in, 0.0});

				s.used_width_in = lane.rip_start_in + p.width_in;
				s.len_in = std::max(s.len_in, p.len_in);
				s.lanes.push_back(std::move(lane));
			} else {
				Lane& lane = s.lanes[best.lane_idx];
				const double offset = lane.used_len_in + kKerfIn;
				lane.placements.push_back(Placement{p, lane.rip_start_in, offset});
				lane.used_len_in = offset + p.len_in;
				s.len_in = std::max(s.len_in, lane.used_len_in);
			}
		}
	}

	return strips;
}

std::vector<PlanStep> buildSequence(std::vector<Strip>& strips, double& total_used_length) {
	// Keep strips in descending length so biggest cross-cuts happen first.
	std::sort(strips.begin(), strips.end(), [](const Strip& a, const Strip& b) {
		return a.len_in > b.len_in;
	});

	total_used_length = 0.0;
	for (size_t i = 0; i < strips.size(); ++i) {
		total_used_length += strips[i].len_in;
		if (i > 0) {
			total_used_length += kKerfIn;
		}
	}

	if (total_used_length > kSheetLengthIn + 1e-9) {
		std::ostringstream oss;
		oss << "Cut list needs " << std::fixed << std::setprecision(2) << total_used_length
			<< " in of sheet length, but only 96.00 in is available.";
		throw std::runtime_error(oss.str());
	}

	std::vector<PlanStep> steps;
	double strip_start = 0.0;
	int seq = 1;

	for (int i = 0; i < static_cast<int>(strips.size()); ++i) {
		Strip& s = strips[i];
		std::sort(s.lanes.begin(), s.lanes.end(), [](const Lane& a, const Lane& b) {
			return a.rip_start_in < b.rip_start_in;
		});

		for (Lane& lane : s.lanes) {
			std::sort(lane.placements.begin(), lane.placements.end(), [](const Placement& a, const Placement& b) {
				return a.strip_offset_in < b.strip_offset_in;
			});
		}

		std::vector<const Placement*> ordered;
		for (const Lane& lane : s.lanes) {
			for (const Placement& p : lane.placements) {
				ordered.push_back(&p);
			}
		}

		std::sort(ordered.begin(), ordered.end(), [](const Placement* a, const Placement* b) {
			if (std::abs(a->strip_offset_in - b->strip_offset_in) > 1e-9) {
				return a->strip_offset_in < b->strip_offset_in;
			}
			return a->rip_start_in < b->rip_start_in;
		});

		for (const Placement* p : ordered) {
			steps.push_back(PlanStep{
				seq++,
				i + 1,
				strip_start,
				s.len_in,
				p->rip_start_in,
				p->strip_offset_in,
				p->piece.id,
				p->piece.copy_index,
				p->piece.len_in,
				p->piece.width_in
			});
		}

		strip_start += s.len_in;
		if (i + 1 < static_cast<int>(strips.size())) {
			strip_start += kKerfIn;
		}
	}

	return steps;
}

void writeCsv(const std::string& path, const std::vector<PlanStep>& steps, double total_used_length) {
	std::ofstream out(path);
	if (!out) {
		throw std::runtime_error("Could not open output file for writing: " + path);
	}

	out << "seq,strip_no,strip_start_in,strip_len_in,rip_start_in,piece_start_in,id,piece_index,len_in,width_in\n";
	out << std::fixed << std::setprecision(3);

	for (const PlanStep& s : steps) {
		out << s.seq << ','
			<< s.strip_no << ','
			<< s.strip_start_in << ','
			<< s.strip_len_in << ','
			<< s.rip_start_in << ','
			<< s.piece_start_in << ','
			<< s.id << ','
			<< s.piece_index << ','
			<< s.len_in << ','
			<< s.width_in << '\n';
	}

	out << "SUMMARY,,,,,,,total_used_length_in," << total_used_length << '\n';
	out << "SUMMARY,,,,,,,leftover_whole_length_in," << (kSheetLengthIn - total_used_length) << '\n';
	out << "SUMMARY,,,,,,,kerf_in," << kKerfIn << '\n';
}

void writeOutput(const std::string& path, const std::vector<PlanStep>& steps, double total_used_length) {
	const std::string ext = toLower(std::filesystem::path(path).extension().string());
	if (ext == ".csv") {
		writeCsv(path, steps, total_used_length);
		return;
	}
	throw std::runtime_error("Unsupported output file extension. Use .csv only");
}

void writeLayoutSvg(const std::string& path, const std::vector<Strip>& strips) {
	std::ofstream out(path);
	if (!out) {
		throw std::runtime_error("Could not open SVG file for writing: " + path);
	}

	const double scale = 8.0;
	const double margin = 40.0;
	const double label_font_size = 9.0;
	const double sheet_w = kSheetWidthIn * scale;
	const double sheet_h = kSheetLengthIn * scale;
	const double canvas_w = sheet_w + margin * 2.0;
	const double canvas_h = sheet_h + margin * 2.0;

	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << canvas_w
		<< "\" height=\"" << canvas_h << "\" viewBox=\"0 0 " << canvas_w << ' ' << canvas_h << "\">\n";
	out << "  <style>\n";
	out << "    .sheet { fill: #f7f4ea; stroke: #111; stroke-width: 2; }\n";
	out << "    .grid { stroke: #d7d2c3; stroke-width: 1; }\n";
	out << "    .piece { fill: #8ecae6; stroke: #044e7a; stroke-width: 1.2; }\n";
	out << "    .scrap { fill: #f4a261; fill-opacity: 0.35; stroke: #9a3412; stroke-width: 1; stroke-dasharray: 4 3; }\n";
	out << "    .stripLine { stroke: #6b7280; stroke-width: 1; stroke-dasharray: 6 4; }\n";
	out << "    .label { font-family: monospace; font-size: " << label_font_size
		<< "px; fill: #111; text-anchor: middle; dominant-baseline: middle; paint-order: stroke; stroke: #f7f4ea; stroke-width: 3; stroke-linejoin: round; }\n";
	out << "    .leftoverLabel { font-family: monospace; font-size: 11px; fill: #111; text-anchor: start; dominant-baseline: auto; paint-order: stroke; stroke: #f7f4ea; stroke-width: 2; }\n";
	out << "    .title { font-family: monospace; font-size: 14px; fill: #111; font-weight: bold; }\n";
	out << "  </style>\n";

	out << "  <text class=\"title\" x=\"" << margin << "\" y=\"" << (margin - 14)
		<< "\">Cut Layout (48in x 96in sheet, cut from top edge, kerf " << kKerfIn << "in)</text>\n";
	out << "  <rect class=\"sheet\" x=\"" << margin << "\" y=\"" << margin
		<< "\" width=\"" << sheet_w << "\" height=\"" << sheet_h << "\" />\n";

	for (int in = 12; in < static_cast<int>(kSheetWidthIn); in += 12) {
		const double gx = margin + in * scale;
		out << "  <line class=\"grid\" x1=\"" << gx << "\" y1=\"" << margin << "\" x2=\""
			<< gx << "\" y2=\"" << (margin + sheet_h) << "\" />\n";
	}
	for (int in = 12; in < static_cast<int>(kSheetLengthIn); in += 12) {
		const double gy = margin + in * scale;
		out << "  <line class=\"grid\" x1=\"" << margin << "\" y1=\"" << gy << "\" x2=\""
			<< (margin + sheet_w) << "\" y2=\"" << gy << "\" />\n";
	}

	double strip_start = 0.0;
	for (size_t strip_i = 0; strip_i < strips.size(); ++strip_i) {
		const Strip& strip = strips[strip_i];
		const double strip_y = margin + strip_start * scale;
		auto formatDim = [](double v) {
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(3) << v;
			std::string s = oss.str();
			while (!s.empty() && s.back() == '0') {
				s.pop_back();
			}
			if (!s.empty() && s.back() == '.') {
				s.pop_back();
			}
			return s;
		};

		if (strip_start > 0.0) {
			out << "  <line class=\"stripLine\" x1=\"" << margin << "\" y1=\"" << strip_y
				<< "\" x2=\"" << (margin + sheet_w) << "\" y2=\"" << strip_y << "\" />\n";
		}

		for (const Lane& lane : strip.lanes) {
			for (const Placement& p : lane.placements) {
				const double x = margin + p.rip_start_in * scale;
				const double y = strip_y + p.strip_offset_in * scale;
				const double w = p.piece.width_in * scale;
				const double h = p.piece.len_in * scale;
				const std::string dims = formatDim(p.piece.len_in) + "x" + formatDim(p.piece.width_in);
				const std::string label = p.piece.id + " (" + dims + ")";
				const double cx = x + (w / 2.0);
				const double cy = y + (h / 2.0);

				out << "  <rect class=\"piece\" x=\"" << x << "\" y=\"" << y
					<< "\" width=\"" << w << "\" height=\"" << h << "\" />\n";

				const double est_char_width = label_font_size * 0.58;
				auto estWidth = [&](const std::string& txt) {
					return est_char_width * static_cast<double>(txt.size());
				};
				const double est_text_width = est_char_width * static_cast<double>(label.size());
				const double horiz_space = w - 10.0;
				const double vert_space = h - 10.0;
				const double line_height = label_font_size + 1.5;

				if (est_text_width <= horiz_space) {
					out << "  <text class=\"label\" x=\"" << cx << "\" y=\"" << cy
						<< "\">" << escapeXml(label) << "</text>\n";
				} else if (est_text_width <= vert_space) {
					out << "  <text class=\"label\" x=\"" << cx << "\" y=\"" << cy
						<< "\" transform=\"rotate(-90 " << cx << ' ' << cy << ")\">"
						<< escapeXml(label) << "</text>\n";
				} else {
					// If one-line text cannot fit, try two-line compact label: id + dimensions.
					const double two_line_width = std::max(estWidth(p.piece.id), estWidth(dims));
					const double two_line_height = line_height * 2.0;
					if (two_line_width <= horiz_space && two_line_height <= vert_space) {
						out << "  <text class=\"label\" x=\"" << cx << "\" y=\"" << (cy - line_height * 0.5) << "\">\n";
						out << "    <tspan x=\"" << cx << "\" dy=\"0\">" << escapeXml(p.piece.id) << "</tspan>\n";
						out << "    <tspan x=\"" << cx << "\" dy=\"" << line_height << "\">" << escapeXml(dims) << "</tspan>\n";
						out << "  </text>\n";
					} else if (two_line_width <= vert_space && two_line_height <= horiz_space) {
						out << "  <text class=\"label\" x=\"" << cx << "\" y=\"" << (cy - line_height * 0.5)
							<< "\" transform=\"rotate(-90 " << cx << ' ' << cy << ")\">\n";
						out << "    <tspan x=\"" << cx << "\" dy=\"0\">" << escapeXml(p.piece.id) << "</tspan>\n";
						out << "    <tspan x=\"" << cx << "\" dy=\"" << line_height << "\">" << escapeXml(dims) << "</tspan>\n";
						out << "  </text>\n";
					} else {
						// If even two-line label cannot fit, keep an id-only label.
					const std::string short_label = p.piece.id;
					const double short_width = estWidth(short_label);
					if (short_width <= horiz_space) {
						out << "  <text class=\"label\" x=\"" << cx << "\" y=\"" << cy
							<< "\">" << escapeXml(short_label) << "</text>\n";
					} else {
						out << "  <text class=\"label\" x=\"" << cx << "\" y=\"" << cy
							<< "\" transform=\"rotate(-90 " << cx << ' ' << cy << ")\">"
							<< escapeXml(short_label) << "</text>\n";
					}
					}
				}
			}

			const double scrap_h = (strip.len_in - lane.used_len_in) * scale;
			if (scrap_h > 0.01) {
				const double x = margin + lane.rip_start_in * scale;
				const double y = strip_y + lane.used_len_in * scale;
				const double w = lane.width_in * scale;
				out << "  <rect class=\"scrap\" x=\"" << x << "\" y=\"" << y
					<< "\" width=\"" << w << "\" height=\"" << scrap_h << "\" />\n";
			}
		}

		strip_start += strip.len_in;
		if (strip_i + 1 < strips.size()) {
			strip_start += kKerfIn;
		}
	}

	if (strip_start < kSheetLengthIn) {
		const double left_y = margin + strip_start * scale;
		const double left_h = (kSheetLengthIn - strip_start) * scale;
		out << "  <rect x=\"" << margin << "\" y=\"" << left_y << "\" width=\"" << sheet_w
			<< "\" height=\"" << left_h << "\" fill=\"#86efac\" fill-opacity=\"0.30\" stroke=\"#166534\" stroke-width=\"1\" />\n";
		out << "  <text class=\"leftoverLabel\" x=\"" << (margin + 6.0) << "\" y=\"" << (left_y + 16.0)
			<< "\">Whole leftover: " << (kSheetLengthIn - strip_start) << "in x 48in</text>\n";
	}

	out << "</svg>\n";
}

std::filesystem::path getExecutableDirectory() {
	try {
		const std::filesystem::path exe_path = std::filesystem::read_symlink("/proc/self/exe");
		if (!exe_path.empty()) {
			return exe_path.parent_path();
		}
	} catch (...) {
		// Fall back to current working directory if /proc/self/exe is unavailable.
	}
	return std::filesystem::current_path();
}

}  // namespace

int main(int argc, char*[]) {
	try {
		if (argc > 1) {
			std::cout << "Note: command-line file arguments are ignored. Using default filenames in executable folder.\n";
		}

		const std::filesystem::path exe_dir = getExecutableDirectory();
		const std::filesystem::path input_path = exe_dir / kDefaultInputFile;
		const std::filesystem::path output_path = exe_dir / kDefaultOutputFile;
		const std::filesystem::path layout_svg_path = exe_dir / kDefaultLayoutSvgFile;

		const std::vector<CutRequest> requests = loadRequests(input_path.string());
		std::vector<Piece> pieces = expandPieces(requests);
		std::vector<Strip> strips = optimizeStrips(std::move(pieces));

		double total_used_length = 0.0;
		const std::vector<PlanStep> steps = buildSequence(strips, total_used_length);

		writeOutput(output_path.string(), steps, total_used_length);
		writeLayoutSvg(layout_svg_path.string(), strips);

		std::cout << std::fixed << std::setprecision(3);
		std::cout << "Optimization complete.\n";
		std::cout << "Input:  " << input_path.string() << "\n";
		std::cout << "Output: " << output_path.string() << "\n";
		std::cout << "Layout: " << layout_svg_path.string() << "\n";
		std::cout << "Total length used from one end: " << total_used_length << " in\n";
		std::cout << "Whole sheet remaining: " << (kSheetLengthIn - total_used_length) << " in x 48.000 in\n";
		return 0;
	} catch (const std::exception& ex) {
		std::cerr << "Error: " << ex.what() << '\n';
		return 2;
	}
}
