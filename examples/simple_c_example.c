#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/bbcpp_simple.h"

int main() {
    const char* test_bbcode = "Hello [b]world[/b]! This is [i]italic[/i] and [u]underlined[/u] text. "
                             "[quote user=Alice]This is a quote from Alice[/quote] "
                             "[code]printf(\"Hello World\");[/code]";
    
    printf("BBCode Simple API Example\n");
    printf("=========================\n\n");
    
    printf("Original BBCode:\n");
    printf("%s\n\n", test_bbcode);
    
    // Test 1: Extract plain text
    char plain_text[1024];
    if (bbcpp_simple_get_text(test_bbcode, plain_text, sizeof(plain_text)) == 0) {
        printf("Plain text extraction:\n");
        printf("'%s'\n\n", plain_text);
    } else {
        printf("Error extracting plain text\n\n");
    }
    
    // Test 2: Convert to HTML
    char html_output[2048];
    if (bbcpp_simple_to_html(test_bbcode, html_output, sizeof(html_output)) == 0) {
        printf("HTML conversion:\n");
        printf("%s\n\n", html_output);
    } else {
        printf("Error converting to HTML\n\n");
    }
    
    // Test 3: Check for specific tags
    printf("Tag analysis:\n");
    printf("Has [b] tags: %s\n", bbcpp_simple_has_tag(test_bbcode, "b") ? "Yes" : "No");
    printf("Has [color] tags: %s\n", bbcpp_simple_has_tag(test_bbcode, "color") ? "Yes" : "No");
    printf("Number of [b] tags: %d\n", bbcpp_simple_count_tags(test_bbcode, "b"));
    printf("Number of [i] tags: %d\n", bbcpp_simple_count_tags(test_bbcode, "i"));
    printf("Number of [quote] tags: %d\n\n", bbcpp_simple_count_tags(test_bbcode, "quote"));
    
    // Test 4: Validate BBCode
    char error_msg[256];
    if (bbcpp_simple_validate(test_bbcode, error_msg, sizeof(error_msg))) {
        printf("BBCode validation: VALID\n");
    } else {
        printf("BBCode validation: INVALID - %s\n", error_msg);
    }
    
    // Test 5: Get statistics
    bbcpp_stats stats;
    if (bbcpp_simple_get_stats(test_bbcode, &stats) == 0) {
        printf("\nBBCode statistics:\n");
        printf("Total tags: %d\n", stats.total_tags);
        printf("Text nodes: %d\n", stats.text_nodes);
        printf("Unique tags: %d\n", stats.unique_tags);
        printf("Max nesting depth: %d\n", stats.max_nesting_depth);
        printf("Total text length: %zu\n", stats.total_text_length);
    }
    
    // Test 6: Strip specific tags
    char stripped[1024];
    if (bbcpp_simple_strip_tag(test_bbcode, "b", stripped, sizeof(stripped)) == 0) {
        printf("\nAfter stripping [b] tags:\n");
        printf("'%s'\n", stripped);
    }
    
    // Test some edge cases
    printf("\nEdge case testing:\n");
    printf("==================\n");
    
    const char* malformed = "[b]unclosed tag";
    char malformed_error[256];
    if (bbcpp_simple_validate(malformed, malformed_error, sizeof(malformed_error))) {
        printf("Malformed BBCode: VALID (unexpected)\n");
    } else {
        printf("Malformed BBCode: INVALID - %s\n", malformed_error);
    }
    
    const char* empty_bbcode = "";
    char empty_text[64];
    if (bbcpp_simple_get_text(empty_bbcode, empty_text, sizeof(empty_text)) == 0) {
        printf("Empty BBCode text: '%s'\n", empty_text);
    }
    
    const char* nested = "[b][i][u]deeply nested[/u][/i][/b]";
    char nested_text[128];
    if (bbcpp_simple_get_text(nested, nested_text, sizeof(nested_text)) == 0) {
        printf("Nested BBCode text: '%s'\n", nested_text);
    }
    
    printf("\nAPI Version: %s\n", bbcpp_simple_version());
    
    printf("\nSimple API example completed!\n");
    return 0;
}