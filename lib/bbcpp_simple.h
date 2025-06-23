#ifndef BBCPP_SIMPLE_H
#define BBCPP_SIMPLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bbcpp_c.h"

/* Simplified high-level functions for common BBCode operations */

/* Parse BBCode and return the raw text content (strips all formatting) */
int bbcpp_simple_get_text(const char* bbcode, char* output, size_t output_size);

/* Parse BBCode and convert to HTML */
int bbcpp_simple_to_html(const char* bbcode, char* output, size_t output_size);

/* Check if BBCode contains specific tags */
int bbcpp_simple_has_tag(const char* bbcode, const char* tag_name);

/* Count occurrences of a specific tag */
int bbcpp_simple_count_tags(const char* bbcode, const char* tag_name);

/* Extract all URLs from BBCode */
int bbcpp_simple_extract_urls(const char* bbcode, char urls[][256], int max_urls);

/* Extract all quote authors */
int bbcpp_simple_extract_quote_authors(const char* bbcode, char authors[][64], int max_authors);

/* Validate BBCode structure (check for matching opening/closing tags) */
int bbcpp_simple_validate(const char* bbcode, char* error_msg, size_t error_msg_size);

/* Strip specific tags while keeping content */
int bbcpp_simple_strip_tag(const char* bbcode, const char* tag_name, char* output, size_t output_size);

/* Replace BBCode with custom formatting */
typedef struct {
    const char* tag_name;
    const char* open_replacement;
    const char* close_replacement;
} bbcpp_tag_replacement;

int bbcpp_simple_replace_tags(const char* bbcode, const bbcpp_tag_replacement* replacements, 
                             int num_replacements, char* output, size_t output_size);

/* Get statistics about BBCode content */
typedef struct {
    int total_tags;
    int text_nodes;
    int unique_tags;
    int max_nesting_depth;
    size_t total_text_length;
} bbcpp_stats;

int bbcpp_simple_get_stats(const char* bbcode, bbcpp_stats* stats);

/* Utility functions */
const char* bbcpp_simple_version(void);
void bbcpp_simple_set_debug(int enable);

#ifdef __cplusplus
}
#endif

#endif /* BBCPP_SIMPLE_H */