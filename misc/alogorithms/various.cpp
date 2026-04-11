// Here is a comprehensive list of C++ standard library algorithms from the <algorithm> header (and related headers like <numeric>). These are grouped by category for easier navigation, based on the latest standard (up to C++23/C++26 where relevant).
// Non-modifying sequence operations
// 
// all_of, any_of, none_of
// for_each, for_each_n
// count, count_if
// mismatch
// find, find_if, find_if_not
// find_end
// find_first_of
// adjacent_find
// search, search_n
// 
// Modifying sequence operations
// Copy operations
// 
// copy, copy_n, copy_if, copy_backward
// move, move_backward
// 
// Swap operations
// swap, swap_ranges, iter_swap
// 
// Transformation operations
// transform
// 
// Generation operations
// fill, fill_n
// generate, generate_n
// 
// Removing operations
// remove, remove_if, remove_copy, remove_copy_if
// 
// Order-changing operations
// reverse, reverse_copy
// rotate, rotate_copy
// shift_left, shift_right (C++20)
// shuffle (replaces random_shuffle)
// 
// Sampling operations
// sample (C++17)
// 
// Partitioning operations
// 
// is_partitioned
// partition, partition_copy, stable_partition
// partition_point
// 
// Sorting operations
// 
// sort, stable_sort, partial_sort, partial_sort_copy
// is_sorted, is_sorted_until
// nth_element
// 
// Binary search operations (on partitioned/sorted ranges)
// 
// lower_bound, upper_bound
// equal_range
// binary_search
// 
// Set operations (on sorted ranges)
// 
// merge, inplace_merge
// includes
// set_union, set_intersection, set_difference, set_symmetric_difference
// 
// Heap operations
// 
// make_heap, push_heap, pop_heap, sort_heap
// is_heap, is_heap_until
// 
// Minimum/maximum operations
// 
// min, max, minmax
// min_element, max_element, minmax_element
// clamp (C++17)
// 
// Lexicographical comparison
// 
// lexicographical_compare
// lexicographical_compare_three_way (C++20)
// 
// Permutation operations
// 
// is_permutation
// next_permutation, prev_permutation
// 
// Numeric operations (from <numeric>)
// 
// iota
// accumulate
// inner_product
// adjacent_difference
// partial_sum
// exclusive_scan, inclusive_scan, transform_exclusive_scan, transform_inclusive_scan (C++17)
// reduce, transform_reduce (C++17)
// gcd, lcm (C++17)
// Fold algorithms in C++23: fold_left, fold_right, fold_left_first, fold_right_last, and their "with iterator" variants
// 
// Operations on uninitialized memory (from <memory>, but often grouped with algorithms)
// 
// uninitialized_copy, uninitialized_copy_n
// uninitialized_fill, uninitialized_fill_n
// uninitialized_move, uninitialized_move_n
// uninitialized_default_construct, uninitialized_default_construct_n
// uninitialized_value_construct, uninitialized_value_construct_n
// destroy, destroy_n, destroy_at

#include <algorithm>
#include <numeric>
#include <vector>
#include <iostream> 





int main() {

    std::vector<int> vec = {2, 4, 6, 8, 10, 11};

    // Example of using std::all_of
    bool all_even = std::all_of(
                                std::begin(vec), 
                                std::end(vec),
                                [](int i){ return i % 2 == 0; }); // Checks if all elements are even numbers

    std::cout << "All elements are even numbers: " << std::boolalpha << all_even << std::endl;


    // Example of find_first_of
    std::vector<std::string> search_vec = {"Bob", "Joe", "Bill", "Larry", "Eve", "Jane"};
    std::vector<std::string> vec_match_candidates = {"Chuck", "Charlie", "Larry", "Joe"};
    auto it = std::find_first_of(
                                std::begin(vec_match_candidates),
                                std::end(vec_match_candidates),
                                std::begin(search_vec),
                                std::end(search_vec)); // Finds the first occurrence of any element from search_vec in vec_match_candidates
    if (it != std::end(vec_match_candidates)) {
        std::cout << "First element from search_vec found in vec_match_candidates: " << *it << std::endl;
    } else {
        std::cout << "No element from search_vec found in vec." << std::endl;
    }

    return 0;
}


