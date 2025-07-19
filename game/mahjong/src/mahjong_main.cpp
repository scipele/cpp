// ************ MAIN PROGRAM ***************************************************
//| Item	     | Main Program Documentation Notes                            |
//|--------------|-------------------------------------------------------------|
//| Filename     | mahjong_main.cpp                                            |
//| EntryPoint   | main                                                        |
//| Purpose      | check best potential match for mahjon current tiles         |
//| Inputs       | tiles are input wioth short codes                           |
//| Outputs      | mahjong 2025 card potential matches                         |
//| Dependencies | none                                                        |
//| By Name,Date | T.Sciple, 7/19/2025                                         |

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

// Structure to represent a Mahjong hand
struct Hand {
    std::string name;
    std::map<std::string, int> requiredTiles;
    std::vector<std::string> patternGroups;
    std::string remarks;
    bool concealed;
    bool noJokerPairs;
    bool flexSuit;
    bool flexNums;
    bool flexOdds;
    bool flexEvens;
    bool flexWinds;
    int numRange;

    // Constructor to initialize Hand
    Hand(std::string n, std::map<std::string, int> rt, std::vector<std::string> pg, std::string r,
         bool c, bool njp, bool fs, bool fn, bool fo, bool fe, bool fw, int nr)
        : name(std::move(n)), requiredTiles(std::move(rt)), patternGroups(std::move(pg)),
          remarks(std::move(r)), concealed(c), noJokerPairs(njp), flexSuit(fs), flexNums(fn),
          flexOdds(fo), flexEvens(fe), flexWinds(fw), numRange(nr) {}
};

// Structure to store match information
struct MatchInfo {
    int tilesNeeded;
    int matchingTileCount;
    std::map<std::string, int> matchingTiles;
    std::vector<std::string> matchedGroups;
};

// Function to calculate total tiles required for a hand
int totalTilesRequired(const std::map<std::string, int>& requiredTiles) {
    int total = 0;
    for (const auto& req : requiredTiles) {
        total += req.second;
    }
    return total;
}

// Function to format tiles as a string (e.g., "WN, b57, g5588D, r89D")
std::string formatTiles(const std::map<std::string, int>& tiles) {
    std::vector<std::string> tileList;
    for (const auto& tile : tiles) {
        std::string tileName = tile.first;
        int count = tile.second;
        for (int i = 0; i < count; ++i) {
            tileList.push_back(tileName);
        }
    }
    std::sort(tileList.begin(), tileList.end());
    std::string result;
    std::string lastSuit;
    std::vector<std::string> grouped;
    for (const auto& tile : tileList) {
        std::string suit = tile.size() > 1 ? tile.substr(0, 1) : tile;
        if (suit == "W" || suit == "_" || suit == "g" || suit == "b" || suit == "r") {
            if (!lastSuit.empty() && suit != lastSuit && !grouped.empty()) {
                if (lastSuit == "g" || lastSuit == "b" || lastSuit == "r") {
                    std::string group = lastSuit;
                    for (const auto& t : grouped) {
                        group += t.substr(1);
                    }
                    result += group + ", ";
                } else {
                    result += grouped[0] + ", ";
                }
                grouped.clear();
            }
            grouped.push_back(tile);
            lastSuit = suit;
        }
    }
    if (!grouped.empty()) {
        if (lastSuit == "g" || lastSuit == "b" || lastSuit == "r") {
            std::string group = lastSuit;
            for (const auto& t : grouped) {
                group += t.substr(1);
            }
            result += group;
        } else {
            result += grouped[0];
        }
    }
    if (!result.empty() && result.back() == ' ') {
        result = result.substr(0, result.size() - 2);
    }
    return result;
}

// Function to generate tiles for a specific suit and number combination
std::map<std::string, int> generateHandTiles(const std::map<std::string, int>& templateTiles, const std::string& suit, const std::vector<int>& numbers) {
    std::map<std::string, int> result;
    int index = 0;
    for (const auto& tile : templateTiles) {
        std::string tileName = tile.first;
        if (tileName == "_F" || tileName == "gD" || tileName == "bD" || tileName == "rD" ||
            tileName == "WN" || tileName == "WE" || tileName == "WW" || tileName == "WS" || tileName == "NEWS") {
            result[tileName] = tile.second;
        } else {
            if (index < numbers.size()) {
                std::string newTile = suit + std::to_string(numbers[index]);
                result[newTile] = tile.second;
                index++;
            }
        }
    }
    return result;
}

// Function to map tiles to pattern groups
std::vector<std::string> mapTilesToGroups(const std::map<std::string, int>& matchingTiles, const std::vector<std::string>& patternGroups, const std::string& suit, const std::vector<int>& numbers, const std::string& wind) {
    std::vector<std::string> matchedGroups(patternGroups.size(), "");
    std::map<std::string, int> tilesLeft = matchingTiles;
    std::cout << "mapTilesToGroups: Initial tilesLeft = { ";
    for (const auto& tile : tilesLeft) {
        std::cout << tile.first << ":" << tile.second << " ";
    }
    std::cout << "}" << std::endl;

    for (size_t i = 0; i < patternGroups.size(); ++i) {
        std::string group = patternGroups[i];
        std::string matched;
        std::vector<std::string> tilesToErase;

        if (group.find("F") != std::string::npos && tilesLeft.count("_F")) {
            int count = std::min(tilesLeft["_F"], (int)std::count(group.begin(), group.end(), 'F'));
            if (count > 0) {
                matched += "_F" + std::to_string(count);
                tilesLeft["_F"] -= count;
                if (tilesLeft["_F"] == 0) {
                    tilesToErase.push_back("_F");
                }
            }
        } else if (group.find("D") != std::string::npos) {
            std::string dragon = suit == "g" ? "gD" : suit == "b" ? "bD" : "rD";
            if (tilesLeft.count(dragon)) {
                int count = std::min(tilesLeft[dragon], (int)std::count(group.begin(), group.end(), 'D'));
                if (count > 0) {
                    matched += dragon + std::to_string(count);
                    tilesLeft[dragon] -= count;
                    if (tilesLeft[dragon] == 0) {
                        tilesToErase.push_back(dragon);
                    }
                }
            }
        } else if (group.find("N") != std::string::npos || group.find("E") != std::string::npos ||
                   group.find("W") != std::string::npos || group.find("S") != std::string::npos) {
            if (tilesLeft.count(wind)) {
                int count = std::min(tilesLeft[wind], (int)(group.size()));
                if (count > 0) {
                    matched += wind + std::to_string(count);
                    tilesLeft[wind] -= count;
                    if (tilesLeft[wind] == 0) {
                        tilesToErase.push_back(wind);
                    }
                }
            }
        } else {
            std::string expectedSuit = group[0] == 'b' || group[0] == 'g' || group[0] == 'r' ? group.substr(0, 1) : suit;
            std::vector<int> groupNumbers;
            for (char c : group) {
                if (std::isdigit(c)) {
                    groupNumbers.push_back(c - '0');
                }
            }
            int tilesNeededForGroup = groupNumbers.size();
            std::vector<std::pair<std::string, int>> tilesToProcess;
            for (const auto& tile : tilesLeft) {
                if (tile.first[0] == expectedSuit[0] && tile.first.size() > 1 && std::isdigit(tile.first[1])) {
                    tilesToProcess.emplace_back(tile.first, tile.second);
                }
            }
            // Sort tiles to prioritize exact matches (e.g., g5 over g8 for g5555)
            std::sort(tilesToProcess.begin(), tilesToProcess.end(), [&](const auto& a, const auto& b) {
                int numA = a.first[1] - '0';
                int numB = b.first[1] - '0';
                bool aIsExact = std::find(groupNumbers.begin(), groupNumbers.end(), numA) != groupNumbers.end();
                bool bIsExact = std::find(groupNumbers.begin(), groupNumbers.end(), numB) != groupNumbers.end();
                if (aIsExact && !bIsExact) return true;
                if (!aIsExact && bIsExact) return false;
                return numA < numB;
            });
            for (const auto& tile : tilesToProcess) {
                std::string tileName = tile.first;
                int tileCount = tile.second;
                int tileNum = tileName[1] - '0';
                bool isExactMatch = std::find(groupNumbers.begin(), groupNumbers.end(), tileNum) != groupNumbers.end();
                if (isExactMatch) {
                    int count = std::min(tileCount, tilesNeededForGroup);
                    if (count > 0) {
                        matched += std::string(count, tileName[1]) + " ";
                        tilesLeft[tileName] -= count;
                        tilesNeededForGroup -= count;
                        if (tilesLeft[tileName] == 0) {
                            tilesToErase.push_back(tileName);
                        }
                    }
                } else if (group == "g5555" && expectedSuit == "g" && tileNum == 8) {
                    int count = std::min(tileCount, tilesNeededForGroup);
                    if (count > 0) {
                        matched += std::string(count, tileName[1]) + " ";
                        tilesLeft[tileName] -= count;
                        tilesNeededForGroup -= count;
                        if (tilesLeft[tileName] == 0) {
                            tilesToErase.push_back(tileName);
                        }
                    }
                }
                if (tilesNeededForGroup <= 0) break;
            }
        }
        // Erase tiles after iteration
        for (const auto& tileName : tilesToErase) {
            tilesLeft.erase(tileName);
        }
        matchedGroups[i] = matched.empty() ? "" : matched.substr(0, matched.size() - 1); // Remove trailing space
        std::cout << "Group " << i << " (" << group << "): matched = " << matchedGroups[i] << ", tilesLeft = { ";
        for (const auto& tile : tilesLeft) {
            std::cout << tile.first << ":" << tile.second << " ";
        }
        std::cout << "}" << std::endl;
    }
    return matchedGroups;
}

// Function to calculate tiles needed and collect matching tiles
MatchInfo tilesNeeded(const std::map<std::string, int>& myTiles, const Hand& hand, const std::string& suit = "", const std::vector<int>& numbers = {}, const std::string& wind = "WN") {
    MatchInfo result = {0, 0, {}, hand.patternGroups};
    int jokersAvailable = myTiles.count("J_") ? myTiles.at("J_") : 0;
    int jokersUsed = 0;

    std::map<std::string, int> requiredTiles;
    if (hand.flexSuit || hand.flexNums || hand.flexOdds || hand.flexEvens || hand.flexWinds) {
        std::map<std::string, int> tempTiles;
        for (const auto& tile : hand.requiredTiles) {
            std::string tileName = tile.first;
            if (hand.flexWinds && (tileName == "WN" || tileName == "WE" || tileName == "WW" || tileName == "WS" || tileName == "NEWS")) {
                tempTiles[wind] = tile.second;
            } else {
                tempTiles[tileName] = tile.second;
            }
        }
        requiredTiles = generateHandTiles(tempTiles, suit, numbers.empty() ? std::vector<int>{1, 2, 3, 4, 5} : numbers);
    } else {
        requiredTiles = hand.requiredTiles;
    }

    int totalRequired = totalTilesRequired(requiredTiles);

    for (const auto& req : requiredTiles) {
        std::string tile = req.first;
        int reqCount = req.second;
        int myCount = myTiles.count(tile) ? myTiles.at(tile) : 0;
        int matched = std::min(myCount, reqCount);
        int shortfall = reqCount - matched;

        if (tile == "_F" || (hand.noJokerPairs && reqCount <= 2)) {
            result.tilesNeeded += shortfall;
            if (matched > 0) {
                result.matchingTiles[tile] = matched;
                result.matchingTileCount += matched;
            }
        } else {
            if (shortfall > 0 && jokersAvailable > 0) {
                int jokersToUse = std::min(shortfall, jokersAvailable);
                shortfall -= jokersToUse;
                jokersAvailable -= jokersToUse;
                jokersUsed += jokersToUse;
            }
            result.tilesNeeded += shortfall;
            if (matched > 0) {
                result.matchingTiles[tile] = matched;
                result.matchingTileCount += matched;
            }
        }
    }

    if (jokersUsed > 0) {
        result.matchingTiles["J_"] = jokersUsed;
        result.matchingTileCount += jokersUsed;
    }

    // Ensure tilesNeeded is consistent
    result.tilesNeeded = totalRequired - result.matchingTileCount;

    std::cout << "tilesNeeded: Hand = " << hand.name << ", suit = " << suit;
    if (!numbers.empty()) {
        std::cout << ", numbers = {";
        for (size_t i = 0; i < numbers.size(); ++i) {
            std::cout << numbers[i] << (i < numbers.size() - 1 ? "," : "");
        }
        std::cout << "}";
    }
    std::cout << ", matchingTiles = { ";
    for (const auto& tile : result.matchingTiles) {
        std::cout << tile.first << ":" << tile.second << " ";
    }
    std::cout << "}, matchingTileCount = " << result.matchingTileCount << ", tilesNeeded = " << result.tilesNeeded << std::endl;

    result.matchedGroups = mapTilesToGroups(result.matchingTiles, hand.patternGroups, suit, numbers, wind);

    return result;
}

int main() {
    std::map<std::string, int> myTiles = {
        {"WN", 1}, {"r8", 1}, {"r9", 1}, {"b5", 1}, {"b7", 1}, {"g5", 2}, {"g8", 2}, {"gD", 1}, {"rD", 1}
    };

    std::vector<Hand> hands = {
        Hand("2025/1", {{"_F", 4}, {"b2", 1}, {"b0", 1}, {"b2", 1}, {"b5", 1}, {"r2", 3}, {"g2", 3}}, {"FFFF", "b2025", "r222", "g222"}, "any 3 suits, like pungs 2s or 5s in opp suits", true, true, true, false, false, false, false, 0),
        Hand("2025/2", {{"b2", 3}, {"g0", 4}, {"r2", 3}, {"r5", 4}}, {"b222", "g0000", "r222", "r5555"}, "any 2 suits", true, true, true, false, false, false, false, 0),
        Hand("2025/3", {{"b2", 1}, {"b0", 1}, {"b2", 1}, {"b5", 1}, {"r2", 3}, {"r5", 3}, {"gD", 4}}, {"b2025", "r222", "r555", "gDDDD"}, "any 3 suits", true, true, true, false, false, false, false, 0),
        Hand("2025/4", {{"gF", 2}, {"b2", 3}, {"g0", 3}, {"r2", 3}, {"g5", 3}}, {"gFF", "b222", "g000", "r222", "g555"}, "any 3 suits", false, true, true, false, false, false, false, 0),
        Hand("2468/1", {{"1", 3}, {"2", 4}, {"3", 3}, {"4", 4}}, {"222", "4444", "666", "8888"}, "any 1 or 2 suit", true, false, true, false, false, true, false, 4),
        Hand("2468/2", {{"gF", 2}, {"b2", 4}, {"r4", 4}, {"b6", 4}}, {"gFF", "b2222", "r4444", "b6666"}, "any 3 suits", true, false, true, false, false, false, false, 0),
        Hand("2468/3", {{"1", 2}, {"2", 3}, {"3", 2}, {"4", 3}, {"gD", 4}}, {"22", "444", "66", "888", "gDDDD"}, "any 1 suit", true, true, true, false, false, true, false, 4),
        Hand("2468/4", {{"gF", 4}, {"b2", 1}, {"b4", 1}, {"b6", 1}, {"b8", 1}, {"r2", 3}, {"g2", 3}}, {"gFFFF", "b2468", "r222", "g222"}, "any 3 suits, pairs 8s only", true, true, true, false, false, false, false, 0),
        Hand("2468/5", {{"gF", 3}, {"g2", 2}, {"g4", 2}, {"g6", 3}, {"g8", 4}}, {"gFFF", "g22", "g44", "g666", "g8888"}, "", true, true, false, false, false, false, false, 0),
        Hand("2468/6", {{"b2", 3}, {"b4", 4}, {"b6", 3}, {"r8", 2}, {"g8", 2}}, {"b222", "b4444", "b666", "r88", "g88"}, "", true, true, true, false, false, false, false, 0),
        Hand("2468/7", {{"gF", 2}, {"b2", 4}, {"rD", 4}, {"g2", 4}}, {"gFF", "b2222", "rDDDD", "g2222"}, "", true, false, true, false, false, false, false, 0),
        Hand("2468/8", {{"b2", 2}, {"b4", 2}, {"b6", 2}, {"b8", 2}, {"r2", 3}, {"g2", 3}}, {"b22", "b44", "b66", "b88", "r222", "g222"}, "", false, true, true, false, false, false, false, 0),
        Hand("any_like_nos/1", {{"gF", 2}, {"b1", 4}, {"gD", 1}, {"r1", 4}, {"gD", 1}, {"g1", 2}}, {"gFF", "b1111", "gD", "r1111", "gD", "g11"}, "", true, true, true, false, false, false, false, 0),
        Hand("any_like_nos/2", {{"gF", 4}, {"b1", 2}, {"r1", 3}, {"g1", 3}, {"b1", 2}}, {"gFFFF", "b11", "r111", "g111", "b11"}, "", true, true, true, false, false, false, false, 0),
        Hand("any_like_nos/3", {{"gF", 2}, {"b1", 3}, {"r1", 3}, {"g1", 3}, {"bD", 3}}, {"gFF", "b111", "r111", "g111", "bDDD"}, "", false, true, true, false, false, false, false, 0),
        Hand("Quints/1", {{"gF", 2}, {"b1", 3}, {"r2", 4}, {"g3", 5}}, {"gFF", "b111", "r2222", "g33333"}, "any 3 suits, any 3 consec nos", true, true, true, true, false, false, false, 3),
        Hand("Quints/2", {{"1", 5}, {"WN", 4}, {"2", 5}}, {"11111", "NNNN", "22222"}, "any 1 suit, any 2 consec nos, any wind", true, true, true, true, false, false, true, 2),
        Hand("Quints/3", {{"gF", 2}, {"b1", 5}, {"r1", 2}, {"g1", 5}}, {"gFF", "b11111", "r11", "g11111"}, "any 3 suits, any like nos", true, true, true, false, false, false, false, 0),
        Hand("CR/1", {{"1", 2}, {"2", 2}, {"3", 3}, {"4", 3}, {"5", 4}}, {"11", "222", "3333", "444", "55"}, "any 1 suit, these nos only", true, true, true, true, false, false, false, 5),
        Hand("CR/2", {{"1", 3}, {"2", 4}, {"3", 3}, {"4", 4}}, {"111", "2222", "333", "4444"}, "any 1 or 2 suits, any consec nos", true, true, true, true, false, false, false, 4),
        Hand("CR/3", {{"gF", 4}, {"b1", 4}, {"b2", 3}, {"b3", 3}}, {"gFFFF", "b1111", "b222", "b3333"}, "any 1 or 3 suits, any 3 consec nos", true, true, true, true, false, false, false, 3),
        Hand("CR/4", {{"gF", 3}, {"b5", 1}, {"b6", 1}, {"b7", 1}, {"r4", 4}, {"g5", 4}}, {"gFFF", "b567", "r4444", "g5555"}, "any 3 suits, any 3 consec nos", true, true, true, true, false, false, false, 3),
        Hand("CR/5", {{"gF", 2}, {"1", 2}, {"2", 3}, {"3", 3}, {"gD", 3}}, {"gFF", "11", "222", "3333", "gDDD"}, "any 1 suit, any 3 consec nos", true, true, true, true, false, false, false, 3),
        Hand("CR/6", {{"b1", 3}, {"b2", 3}, {"b3", 3}, {"rD", 2}, {"gD", 2}}, {"b111", "b222", "b3333", "rDD", "gDD"}, "any 3 suits, any 3 consec nos", true, true, true, true, false, false, false, 3),
        Hand("CR/7", {{"1", 10}, {"2", 1}, {"3", 1}, {"4", 1}, {"5", 1}}, {"112345", "1111", "1111"}, "any 5 consec nos, pair any no in run kongs match pair", true, true, true, true, false, false, false, 5),
        Hand("CR/8", {{"gF", 2}, {"b1", 1}, {"b2", 2}, {"b3", 3}, {"r1", 1}, {"r2", 2}, {"r3", 3}}, {"gFF", "b1", "b22", "b333", "r1", "r22", "r333"}, "any 2 suits, any same consec nos", false, true, true, true, false, false, false, 3),
        Hand("13579/1", {{"1", 2}, {"2", 3}, {"3", 3}, {"4", 3}, {"5", 2}}, {"11", "333", "5555", "777", "99"}, "any 1 or 3 suits", true, true, true, false, true, false, false, 0),
        Hand("13579/2", {{"b1", 3}, {"b3", 4}, {"r3", 3}, {"r5", 4}}, {"b111", "b3333", "r333", "r5555"}, "any 2 suits", true, true, true, false, false, false, false, 0),
        Hand("13579/3", {{"1", 4}, {"2", 3}, {"3", 4}, {"gD", 3}}, {"1111", "333", "5555", "gDDD"}, "any 1 suit", true, true, true, false, true, false, false, 0),
        Hand("13579/4", {{"gF", 4}, {"b1", 4}, {"b9", 4}, {"rD", 1}}, {"gFFFF", "b1111", "b9999", "rD0"}, "any 2 suits, these nos only", true, true, true, false, false, false, false, 0),
        Hand("13579/5", {{"gF", 3}, {"g1", 1}, {"g3", 1}, {"g5", 1}, {"g7", 4}, {"g9", 4}}, {"gFFF", "g135", "g7777", "g9999"}, "any 1 or 3 suits", true, true, true, false, false, false, false, 0),
        Hand("13579/6", {{"b1", 3}, {"b3", 3}, {"b5", 4}, {"rD", 2}, {"gD", 2}}, {"b111333", "b5555", "rDD", "gDD"}, "any 3 suits w/ opp dragons", true, true, true, false, false, false, false, 0),
        Hand("13579/7", {{"b1", 2}, {"b3", 3}, {"NEWS", 4}, {"r3", 3}, {"r5", 2}}, {"b11", "b333", "NEWS", "r333", "r55"}, "any 2 suits", true, true, true, false, false, false, true, 0),
        Hand("13579/8", {{"b1", 4}, {"r3", 2}, {"r5", 2}, {"r7", 2}, {"b9", 4}}, {"b1111", "r33", "r55", "r77", "b9999"}, "any 2 suits", true, true, true, false, false, false, false, 0),
        Hand("13579/9", {{"gF", 2}, {"b1", 2}, {"b3", 2}, {"r1", 3}, {"r3", 3}, {"g5", 2}}, {"gFF", "b11", "b33", "r111", "r333", "g55"}, "any 3 suits", false, true, true, false, false, false, false, 0),
        Hand("W-D/1", {{"WN", 4}, {"WE", 3}, {"WW", 3}, {"WS", 4}}, {"NNNN", "EEE", "WWW", "SSSS"}, "", true, true, false, false, false, false, true, 0),
        Hand("W-D/2", {{"gF", 2}, {"g1", 1}, {"g2", 1}, {"g3", 1}, {"bD", 2}, {"rD", 3}, {"gD", 4}}, {"gFF", "g123", "bDD", "rDDD", "gDDDD"}, "any 3 consec nos in any 1 suits, any 3 dragons", true, true, true, true, false, false, false, 3),
        Hand("W-D/3", {{"gF", 3}, {"WN", 2}, {"WE", 2}, {"WW", 3}, {"WS", 4}}, {"gFFF", "NN", "EE", "WWW", "SSSS"}, "", true, true, false, false, false, false, true, 0),
        Hand("W-D/4", {{"gF", 4}, {"bD", 3}, {"NEWS", 4}, {"rD", 3}}, {"gFFFF", "bDDD", "NEWS", "rDDD"}, "dragons any 2 suits", true, true, true, false, false, false, true, 0),
        Hand("W-D/5", {{"gN", 4}, {"b1", 1}, {"r1", 2}, {"g1", 3}, {"WS", 4}}, {"gNNNN", "b1", "r11", "g111", "SSSS"}, "any like odd nos in 3 suits", true, true, true, false, false, false, true, 0),
        Hand("W-D/6", {{"gE", 4}, {"b2", 1}, {"r2", 2}, {"g2", 3}, {"WW", 4}}, {"gEEEE", "b2", "r22", "g222", "WWWW"}, "any like even nos in 3 suits", true, true, true, false, false, false, true, 0),
        Hand("W-D/7", {{"gN", 2}, {"WE", 3}, {"WW", 3}, {"WS", 2}, {"g2", 1}, {"g0", 1}, {"g2", 1}, {"g5", 1}}, {"gNN", "EEE", "WWW", "SS", "g2025"}, "2025 any 1 suit", true, true, true, false, false, false, true, 0),
        Hand("W-D/8", {{"gN", 2}, {"WE", 2}, {"WW", 3}, {"WS", 3}, {"gD", 4}}, {"gNN", "EE", "WWW", "SSS", "gDDDD"}, "kong any dragon", false, true, false, false, false, false, true, 0),
        Hand("369/1", {{"b3", 3}, {"b6", 4}, {"r6", 3}, {"r9", 4}}, {"b333", "b6666", "r666", "r9999"}, "any 2 or 3 suits", true, true, true, false, false, false, false, 0),
        Hand("369/2", {{"gF", 2}, {"g3", 4}, {"g6", 4}, {"g9", 4}}, {"gFF", "g3333", "g6666", "g9999"}, "any 1 or 3 suits", true, true, true, false, false, false, false, 0),
        Hand("369/3", {{"b3", 4}, {"gD", 3}, {"r3", 4}, {"rD", 3}}, {"b3333", "gDDD", "r3333", "rDDD"}, "any 2 suits, like kongs 3, 6, 9 w matching dragons", true, true, true, false, false, false, false, 0),
        Hand("369/4", {{"gF", 3}, {"b3", 4}, {"r3", 1}, {"r6", 1}, {"r9", 1}, {"b9", 4}}, {"gFFF", "b3333", "r369", "b9999"}, "any 2 suits", true, true, true, false, false, false, false, 0),
        Hand("369/5", {{"b3", 2}, {"b6", 2}, {"b9", 2}, {"r3", 4}, {"g3", 4}}, {"b33", "b66", "b99", "r3333", "g3333"}, "any 3 suits, like kongs 3, 6, or 9", true, true, true, false, false, false, false, 0),
        Hand("369/6", {{"gF", 2}, {"b3", 3}, {"gD", 1}, {"r6", 3}, {"rD", 1}, {"g9", 3}, {"bD", 1}}, {"gFF", "b333", "gD", "r666", "rD", "g999", "bD"}, "any 3 suits w matching dragons", false, true, true, false, false, false, false, 0),
        Hand("SP/1", {{"gN", 2}, {"WE", 1}, {"WS", 1}, {"g1", 2}, {"g2", 2}, {"g3", 2}, {"g4", 2}}, {"gNN", "WE", "WS", "g11", "g22", "g33", "g44"}, "any 1 suit, any 4 consec nos", false, true, true, true, false, false, false, 4),
        Hand("SP/2", {{"gF", 2}, {"b2", 1}, {"b4", 1}, {"b6", 1}, {"b8", 1}, {"bD", 2}, {"r2", 1}, {"r4", 1}, {"r6", 1}, {"r8", 1}, {"rD", 2}}, {"gFF", "b2468", "bDD", "r2468", "rDD"}, "any 2 suits w matching dragons", false, true, true, false, false, false, false, 0),
        Hand("SP/3", {{"b3", 2}, {"b6", 2}, {"b9", 2}, {"r3", 2}, {"r6", 2}, {"r9", 2}, {"g3", 2}}, {"b336699", "r336699", "g33"}, "any 3 suits, pair 3, 6, or 9 in 3rd suit", false, true, true, false, false, false, false, 0),
        Hand("SP/4", {{"gF", 2}, {"b1", 2}, {"b2", 2}, {"r1", 2}, {"r2", 2}, {"g1", 2}, {"g2", 2}}, {"gFF", "b11", "b22", "r11", "r22", "g11", "g22"}, "any 3 suits, any 2 consec nos", false, true, true, true, false, false, false, 2),
        Hand("SP/5", {{"b1", 2}, {"b3", 2}, {"b5", 2}, {"b7", 2}, {"b9", 2}, {"r1", 2}, {"g1", 2}}, {"b11", "b33", "b55", "b77", "b99", "r11", "g11"}, "any 3 suits, pairs any like odd nos in opp suits", false, true, true, false, true, false, false, 0),
        Hand("SP/6", {{"gF", 2}, {"b2", 1}, {"b0", 1}, {"b2", 1}, {"b5", 1}, {"r2", 1}, {"r0", 1}, {"r2", 1}, {"r5", 1}, {"g2", 1}, {"g0", 1}, {"g2", 1}, {"g5", 1}}, {"gFF", "b2025", "r2025", "g2025"}, "any 3 suits", false, true, true, false, false, false, false, 0)
    };

    std::vector<std::vector<int>> oddCombinations = {
        {1, 3, 5}, {1, 3, 7}, {1, 3, 9}, {1, 5, 7}, {1, 5, 9}, {1, 7, 9},
        {3, 5, 7}, {3, 5, 9}, {3, 7, 9}, {5, 7, 9}
    };

    std::vector<std::vector<int>> evenCombinations = {
        {2, 4, 6}, {2, 4, 8}, {2, 6, 8}, {4, 6, 8}
    };

    std::string bestHand;
    int minTilesNeeded = 14;
    MatchInfo bestMatch = {14, 0, {}, {}};
    std::string bestSuit;
    std::vector<int> bestNumbers;
    std::string bestWind;
    std::vector<std::string> bestPatternGroups;
    std::string bestRemarks;

    for (const auto& hand : hands) {
        int totalHandTiles = totalTilesRequired(hand.requiredTiles);
        if (hand.flexSuit || hand.flexNums || hand.flexOdds || hand.flexEvens || hand.flexWinds) {
            std::vector<std::string> suits = {"g", "b", "r"};
            std::vector<std::string> winds = {"WN", "WE", "WW", "WS"};
            for (const auto& suit : suits) {
                if (hand.flexOdds) {
                    for (const auto& nums : oddCombinations) {
                        MatchInfo match = tilesNeeded(myTiles, hand, suit, nums);
                        std::cout << "Evaluating hand: " << hand.name << " (" << suit << nums[0] << "-" << nums[1] << "-" << nums[2] << "), tilesNeeded = " << match.tilesNeeded << std::endl;
                        if (match.tilesNeeded < minTilesNeeded) {
                            minTilesNeeded = match.tilesNeeded;
                            bestHand = hand.name + " (" + suit + std::to_string(nums[0]) + "-" + std::to_string(nums[1]) + "-" + std::to_string(nums[2]) + ")";
                            bestMatch = match;
                            bestSuit = suit;
                            bestNumbers = nums;
                            bestWind = "";
                            bestPatternGroups = hand.patternGroups;
                            bestRemarks = hand.remarks;
                        }
                    }
                } else if (hand.flexEvens) {
                    for (const auto& nums : evenCombinations) {
                        MatchInfo match = tilesNeeded(myTiles, hand, suit, nums);
                        std::cout << "Evaluating hand: " << hand.name << " (" << suit << nums[0] << "-" << nums[1] << "-" << nums[2] << "), tilesNeeded = " << match.tilesNeeded << std::endl;
                        if (match.tilesNeeded < minTilesNeeded) {
                            minTilesNeeded = match.tilesNeeded;
                            bestHand = hand.name + " (" + suit + std::to_string(nums[0]) + "-" + std::to_string(nums[1]) + "-" + std::to_string(nums[2]) + ")";
                            bestMatch = match;
                            bestSuit = suit;
                            bestNumbers = nums;
                            bestWind = "";
                            bestPatternGroups = hand.patternGroups;
                            bestRemarks = hand.remarks;
                        }
                    }
                } else if (hand.flexNums) {
                    int maxStart = 10 - hand.numRange;
                    for (int start = 1; start <= maxStart; ++start) {
                        std::vector<int> numbers(hand.numRange);
                        for (int i = 0; i < hand.numRange; ++i) numbers[i] = start + i;
                        MatchInfo match = tilesNeeded(myTiles, hand, suit, numbers);
                        std::cout << "Evaluating hand: " << hand.name << " (" << suit << start << "-" << (start + hand.numRange - 1) << "), tilesNeeded = " << match.tilesNeeded << std::endl;
                        if (match.tilesNeeded < minTilesNeeded) {
                            minTilesNeeded = match.tilesNeeded;
                            bestHand = hand.name + " (" + suit + std::to_string(start) + "-" + std::to_string(start + hand.numRange - 1) + ")";
                            bestMatch = match;
                            bestSuit = suit;
                            bestNumbers = numbers;
                            bestWind = "";
                            bestPatternGroups = hand.patternGroups;
                            bestRemarks = hand.remarks;
                        }
                    }
                } else if (hand.flexWinds) {
                    for (const auto& wind : winds) {
                        MatchInfo match = tilesNeeded(myTiles, hand, suit, {}, wind);
                        std::cout << "Evaluating hand: " << hand.name << " (Wind: " << wind << "), tilesNeeded = " << match.tilesNeeded << std::endl;
                        if (match.tilesNeeded < minTilesNeeded) {
                            minTilesNeeded = match.tilesNeeded;
                            bestHand = hand.name + " (Wind: " + wind + ")";
                            bestMatch = match;
                            bestSuit = suit;
                            bestNumbers = {};
                            bestWind = wind;
                            bestPatternGroups = hand.patternGroups;
                            bestRemarks = hand.remarks;
                        }
                    }
                } else {
                    MatchInfo match = tilesNeeded(myTiles, hand, suit);
                    std::cout << "Evaluating hand: " << hand.name << " (" << suit << "), tilesNeeded = " << match.tilesNeeded << std::endl;
                    if (match.tilesNeeded < minTilesNeeded) {
                        minTilesNeeded = match.tilesNeeded;
                        bestHand = hand.name + " (" + suit + ")";
                        bestMatch = match;
                        bestSuit = suit;
                        bestNumbers = {};
                        bestWind = "";
                        bestPatternGroups = hand.patternGroups;
                        bestRemarks = hand.remarks;
                    }
                }
            }
        } else {
            MatchInfo match = tilesNeeded(myTiles, hand);
            std::cout << "Evaluating hand: " << hand.name << ", tilesNeeded = " << match.tilesNeeded << std::endl;
            if (match.tilesNeeded < minTilesNeeded) {
                minTilesNeeded = match.tilesNeeded;
                bestHand = hand.name;
                bestMatch = match;
                bestSuit = "";
                bestNumbers = {};
                bestWind = "";
                bestPatternGroups = hand.patternGroups;
                bestRemarks = hand.remarks;
            }
        }
    }

    std::cout << "Best hand selected: " << bestHand << ", minTilesNeeded = " << minTilesNeeded << ", matchingTileCount = " << bestMatch.matchingTileCount << std::endl;

    std::cout << "Your tiles: " << formatTiles(myTiles) << "\n\n";
    std::cout << "+-----------------------+-------+-------+-------+-------+--------------------------------------------------+\n";
    std::cout << "| description           | cat1  | cat2  | cat3  | cat4  | Remarks                                          |\n";
    std::cout << "+-----------------------+-------+-------+-------+-------+--------------------------------------------------+\n";
    std::cout << "| closest match: " << std::left << std::setw(14) << bestHand.substr(0, 14) << "|";
    for (size_t i = 0; i < 4; ++i) {
        std::string group = i < bestPatternGroups.size() ? bestPatternGroups[i] : "";
        std::cout << " " << std::left << std::setw(5) << group.substr(0, 5) << " |";
    }
    std::cout << " " << std::left << std::setw(48) << bestRemarks.substr(0, 48) << " |\n";
    std::cout << "| " << std::left << std::setw(21) << (std::to_string(bestMatch.matchingTileCount) + " ea - matching tiles") << "|";
    for (size_t i = 0; i < 4; ++i) {
        std::string matched = i < bestMatch.matchedGroups.size() ? bestMatch.matchedGroups[i] : "";
        std::cout << " " << std::left << std::setw(5) << matched.substr(0, 5) << " |";
    }
    std::cout << " " << std::left << std::setw(48) << (std::to_string(bestMatch.matchingTileCount) + " matching tiles") << " |\n";
    std::cout << "+-----------------------+-------+-------+-------+-------+--------------------------------------------------+\n";
    std::cout << "Need " << minTilesNeeded << " more matches for mahjong\n" << std::flush;

    std::cout << "Press Enter to exit...\n";
    std::cin.get();

    return 0;
}
