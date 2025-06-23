#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/bbcpp_c.h"

void print_node_recursive(bbcpp_node_handle node, int depth) {
    if (!node) return;
    
    // Print indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    // Get node type
    bbcpp_node_type type;
    if (bbcpp_node_get_type(node, &type) != BBCPP_SUCCESS) {
        printf("Error getting node type\n");
        return;
    }
    
    // Get node name
    char name[256] = {0};
    size_t name_length;
    bbcpp_error result = bbcpp_node_get_name(node, name, sizeof(name), &name_length);
    if (result != BBCPP_SUCCESS) {
        printf("Error getting node name: %s\n", bbcpp_error_string(result));
        return;
    }
    
    switch (type) {
        case BBCPP_NODE_DOCUMENT:
            printf("[DOCUMENT] %s\n", name);
            break;
            
        case BBCPP_NODE_ELEMENT: {
            printf("[ELEMENT] %s", name);
            
            // Check if it has parameters
            size_t param_count;
            if (bbcpp_element_get_parameter_count(node, &param_count) == BBCPP_SUCCESS && param_count > 0) {
                printf(" (");
                for (size_t i = 0; i < param_count; i++) {
                    char key[128] = {0}, value[128] = {0};
                    size_t key_len, value_len;
                    
                    if (bbcpp_element_get_parameter_by_index(node, i, 
                                                           key, sizeof(key), &key_len,
                                                           value, sizeof(value), &value_len) == BBCPP_SUCCESS) {
                        if (i > 0) printf(", ");
                        printf("%s=%s", key, value);
                    }
                }
                printf(")");
            }
            printf("\n");
            break;
        }
        
        case BBCPP_NODE_TEXT: {
            char content[512] = {0};
            size_t content_length;
            if (bbcpp_text_get_content(node, content, sizeof(content), &content_length) == BBCPP_SUCCESS) {
                printf("[TEXT] \"%s\"\n", content);
            } else {
                printf("[TEXT] <error getting content>\n");
            }
            break;
        }
        
        default:
            printf("[UNKNOWN] %s\n", name);
            break;
    }
    
    // Print children
    size_t child_count;
    if (bbcpp_node_get_children_count(node, &child_count) == BBCPP_SUCCESS) {
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_node_get_child(node, i, &child) == BBCPP_SUCCESS) {
                print_node_recursive(child, depth + 1);
                // Note: In a real application, you'd need to track and free these node handles
                // For this simple example, we're not doing proper cleanup
            }
        }
    }
}

int main(int argc, char* argv[]) {
    const char* test_bbcode = "This is [b]bold text[/b] and [i]italic text[/i]. "
                             "[quote user=John]This is a quote[/quote] "
                             "Normal text again.";
    
    printf("BBCode C Interface Example\n");
    printf("=========================\n\n");
    
    // Create document
    bbcpp_document_handle doc = bbcpp_document_create();
    if (!doc) {
        printf("Error: Could not create document\n");
        return 1;
    }
    
    printf("Input BBCode:\n%s\n\n", test_bbcode);
    
    // Load BBCode
    bbcpp_error result = bbcpp_document_load(doc, test_bbcode);
    if (result != BBCPP_SUCCESS) {
        printf("Error loading BBCode: %s\n", bbcpp_error_string(result));
        bbcpp_document_destroy(doc);
        return 1;
    }
    
    printf("Parsed Structure:\n");
    printf("-----------------\n");
    
    // Print using built-in function
    bbcpp_document_print(doc);
    
    printf("\nManual Tree Walk:\n");
    printf("-----------------\n");
    
    // Get document as a node and walk the tree
    size_t child_count;
    if (bbcpp_document_get_children_count(doc, &child_count) == BBCPP_SUCCESS) {
        printf("Document has %zu children:\n", child_count);
        
        for (size_t i = 0; i < child_count; i++) {
            bbcpp_node_handle child;
            if (bbcpp_document_get_child(doc, i, &child) == BBCPP_SUCCESS) {
                print_node_recursive(child, 0);
            }
        }
    }
    
    printf("\nRaw Text Extraction:\n");
    printf("-------------------\n");
    
    // Extract raw text from each child
    for (size_t i = 0; i < child_count; i++) {
        bbcpp_node_handle child;
        if (bbcpp_document_get_child(doc, i, &child) == BBCPP_SUCCESS) {
            char raw_text[1024] = {0};
            size_t raw_length;
            
            if (bbcpp_get_raw_string(child, raw_text, sizeof(raw_text), &raw_length) == BBCPP_SUCCESS) {
                printf("Child %zu raw text: \"%s\"\n", i, raw_text);
            }
        }
    }
    
    // Clean up
    bbcpp_document_destroy(doc);
    
    printf("\nExample completed successfully!\n");
    return 0;
}