#include "bbcpp_simple.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int debug_enabled = 0;

/* Helper function to walk nodes and extract text */
static void extract_text_recursive(bbcpp_node_handle node, char* buffer, size_t* pos, size_t max_size) {
    if (!node || *pos >= max_size - 1) return;
    
    bbcpp_node_type type;
    if (bbcpp_node_get_type(node, &type) != BBCPP_SUCCESS) return;
    
    if (type == BBCPP_NODE_TEXT) {
        char content[512];
        size_t content_length;
        if (bbcpp_text_get_content(node, content, sizeof(content), &content_length) == BBCPP_SUCCESS) {
            size_t copy_len = (content_length < max_size - *pos - 1) ? content_length : max_size - *pos - 1;
            memcpy(buffer + *pos, content, copy_len);
            *pos += copy_len;
        }
    }
    
    /* Recursively process children */
    size_t child_count;
    if (bbcpp_node_get_children_count(node, &child_count) == BBCPP_SUCCESS) {
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_node_get_child(node, i, &child) == BBCPP_SUCCESS) {
                extract_text_recursive(child, buffer, pos, max_size);
            }
        }
    }
}

/* Helper function to count tags recursively */
static int count_tags_recursive(bbcpp_node_handle node, const char* tag_name) {
    if (!node) return 0;
    
    int count = 0;
    bbcpp_node_type type;
    if (bbcpp_node_get_type(node, &type) != BBCPP_SUCCESS) return 0;
    
    if (type == BBCPP_NODE_ELEMENT) {
        char name[64];
        size_t name_length;
        if (bbcpp_node_get_name(node, name, sizeof(name), &name_length) == BBCPP_SUCCESS) {
            if (strcmp(name, tag_name) == 0) {
                count++;
            }
        }
    }
    
    /* Recursively process children */
    size_t child_count;
    if (bbcpp_node_get_children_count(node, &child_count) == BBCPP_SUCCESS) {
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_node_get_child(node, i, &child) == BBCPP_SUCCESS) {
                count += count_tags_recursive(child, tag_name);
            }
        }
    }
    
    return count;
}

/* Helper function to convert node to HTML recursively */
static void to_html_recursive(bbcpp_node_handle node, char* buffer, size_t* pos, size_t max_size) {
    if (!node || *pos >= max_size - 1) return;
    
    bbcpp_node_type type;
    if (bbcpp_node_get_type(node, &type) != BBCPP_SUCCESS) return;
    
    if (type == BBCPP_NODE_TEXT) {
        char content[512];
        size_t content_length;
        if (bbcpp_text_get_content(node, content, sizeof(content), &content_length) == BBCPP_SUCCESS) {
            size_t copy_len = (content_length < max_size - *pos - 1) ? content_length : max_size - *pos - 1;
            memcpy(buffer + *pos, content, copy_len);
            *pos += copy_len;
        }
    } else if (type == BBCPP_NODE_ELEMENT) {
        char name[64];
        size_t name_length;
        if (bbcpp_node_get_name(node, name, sizeof(name), &name_length) == BBCPP_SUCCESS) {
            /* Handle common BBCode to HTML conversions */
            const char* html_open = "";
            const char* html_close = "";
            
            if (strcmp(name, "b") == 0) {
                html_open = "<strong>";
                html_close = "</strong>";
            } else if (strcmp(name, "i") == 0) {
                html_open = "<em>";
                html_close = "</em>";
            } else if (strcmp(name, "u") == 0) {
                html_open = "<u>";
                html_close = "</u>";
            } else if (strcmp(name, "code") == 0) {
                html_open = "<code>";
                html_close = "</code>";
            } else if (strcmp(name, "quote") == 0) {
                html_open = "<blockquote>";
                html_close = "</blockquote>";
            } else if (strcmp(name, "url") == 0) {
                html_open = "<a href=\"#\">";
                html_close = "</a>";
            }
            
            /* Add opening tag */
            size_t open_len = strlen(html_open);
            if (*pos + open_len < max_size - 1) {
                memcpy(buffer + *pos, html_open, open_len);
                *pos += open_len;
            }
            
            /* Process children */
            size_t child_count;
            if (bbcpp_node_get_children_count(node, &child_count) == BBCPP_SUCCESS) {
                for (size_t i = 0; i < child_count; i++) {
                    bbcpp_node_handle child;
                    if (bbcpp_node_get_child(node, i, &child) == BBCPP_SUCCESS) {
                        bbcpp_node_type child_type;
                        if (bbcpp_node_get_type(child, &child_type) == BBCPP_SUCCESS && 
                            child_type != BBCPP_NODE_ELEMENT) {
                            to_html_recursive(child, buffer, pos, max_size);
                        }
                    }
                }
            }
            
            /* Add closing tag */
            size_t close_len = strlen(html_close);
            if (*pos + close_len < max_size - 1) {
                memcpy(buffer + *pos, html_close, close_len);
                *pos += close_len;
            }
        }
    }
}

/* Implementation of public functions */

int bbcpp_simple_get_text(const char* bbcode, char* output, size_t output_size) {
    if (!bbcode || !output || output_size == 0) return -1;
    
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) return -1;
    
    if (bbcpp_document_load(doc, bbcode) != BBCPP_SUCCESS) {
        bbcpp_document_destroy(doc);
        return -1;
    }
    
    output[0] = '\0';
    size_t pos = 0;
    
    size_t child_count;
    if (bbcpp_document_get_children_count(doc, &child_count) == BBCPP_SUCCESS) {
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_document_get_child(doc, i, &child) == BBCPP_SUCCESS) {
                extract_text_recursive(child, output, &pos, output_size);
            }
        }
    }
    
    if (pos < output_size) {
        output[pos] = '\0';
    } else {
        output[output_size - 1] = '\0';
    }
    
    bbcpp_document_destroy(doc);
    return 0;
}

int bbcpp_simple_to_html(const char* bbcode, char* output, size_t output_size) {
    if (!bbcode || !output || output_size == 0) return -1;
    
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) return -1;
    
    if (bbcpp_document_load(doc, bbcode) != BBCPP_SUCCESS) {
        bbcpp_document_destroy(doc);
        return -1;
    }
    
    output[0] = '\0';
    size_t pos = 0;
    
    size_t child_count;
    if (bbcpp_document_get_children_count(doc, &child_count) == BBCPP_SUCCESS) {
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_document_get_child(doc, i, &child) == BBCPP_SUCCESS) {
                to_html_recursive(child, output, &pos, output_size);
            }
        }
    }
    
    if (pos < output_size) {
        output[pos] = '\0';
    } else {
        output[output_size - 1] = '\0';
    }
    
    bbcpp_document_destroy(doc);
    return 0;
}

int bbcpp_simple_has_tag(const char* bbcode, const char* tag_name) {
    if (!bbcode || !tag_name) return 0;
    
    return bbcpp_simple_count_tags(bbcode, tag_name) > 0;
}

int bbcpp_simple_count_tags(const char* bbcode, const char* tag_name) {
    if (!bbcode || !tag_name) return 0;
    
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) return 0;
    
    if (bbcpp_document_load(doc, bbcode) != BBCPP_SUCCESS) {
        bbcpp_document_destroy(doc);
        return 0;
    }
    
    int total_count = 0;
    size_t child_count;
    
    if (bbcpp_document_get_children_count(doc, &child_count) == BBCPP_SUCCESS) {
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_document_get_child(doc, i, &child) == BBCPP_SUCCESS) {
                total_count += count_tags_recursive(child, tag_name);
            }
        }
    }
    
    bbcpp_document_destroy(doc);
    return total_count;
}

int bbcpp_simple_extract_urls(const char* bbcode, char urls[][256], int max_urls) {
    /* This would require more complex parsing to extract URLs from url tags */
    /* For now, return 0 indicating no URLs found */
    (void)bbcode; (void)urls; (void)max_urls; /* Suppress unused parameter warnings */
    return 0;
}

int bbcpp_simple_extract_quote_authors(const char* bbcode, char authors[][64], int max_authors) {
    if (!bbcode || !authors || max_authors <= 0) return 0;
    
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) return 0;
    
    if (bbcpp_document_load(doc, bbcode) != BBCPP_SUCCESS) {
        bbcpp_document_destroy(doc);
        return 0;
    }
    
    int author_count = 0;
    /* This would require walking the tree and finding quote elements with user parameters */
    /* Implementation left as exercise - would be similar to count_tags_recursive */
    
    bbcpp_document_destroy(doc);
    return author_count;
}

int bbcpp_simple_validate(const char* bbcode, char* error_msg, size_t error_msg_size) {
    if (!bbcode) return 0;
    
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) {
        if (error_msg && error_msg_size > 0) {
            strncpy(error_msg, "Failed to create document", error_msg_size - 1);
            error_msg[error_msg_size - 1] = '\0';
        }
        return 0;
    }
    
    bbcpp_error result = bbcpp_document_load(doc, bbcode);
    bbcpp_document_destroy(doc);
    
    if (result != BBCPP_SUCCESS) {
        if (error_msg && error_msg_size > 0) {
            strncpy(error_msg, bbcpp_error_string(result), error_msg_size - 1);
            error_msg[error_msg_size - 1] = '\0';
        }
        return 0;
    }
    
    return 1; /* Valid */
}

int bbcpp_simple_strip_tag(const char* bbcode, const char* tag_name, char* output, size_t output_size) {
    /* This would require filtering out specific tags while keeping content */
    /* For now, just copy the raw text */
    return bbcpp_simple_get_text(bbcode, output, output_size);
}

int bbcpp_simple_replace_tags(const char* bbcode, const bbcpp_tag_replacement* replacements, 
                             int num_replacements, char* output, size_t output_size) {
    /* This would require custom tag replacement logic */
    /* For now, just return the original text */
    (void)replacements; (void)num_replacements; /* Suppress unused parameter warnings */
    return bbcpp_simple_get_text(bbcode, output, output_size);
}

int bbcpp_simple_get_stats(const char* bbcode, bbcpp_stats* stats) {
    if (!bbcode || !stats) return -1;
    
    memset(stats, 0, sizeof(bbcpp_stats));
    
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) return -1;
    
    if (bbcpp_document_load(doc, bbcode) != BBCPP_SUCCESS) {
        bbcpp_document_destroy(doc);
        return -1;
    }
    
    /* Basic stats - could be expanded with recursive tree walking */
    size_t child_count;
    if (bbcpp_document_get_children_count(doc, &child_count) == BBCPP_SUCCESS) {
        stats->total_tags = (int)child_count;
    }
    
    bbcpp_document_destroy(doc);
    return 0;
}

const char* bbcpp_simple_version(void) {
    return "BBCode C Simple API v1.0";
}

void bbcpp_simple_set_debug(int enable) {
    debug_enabled = enable;
}