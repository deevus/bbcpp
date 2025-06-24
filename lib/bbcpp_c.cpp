#include "bbcpp_c.h"
#include "BBDocument.h"
#include "bbcpputils.h"
#include <cstring>
#include <memory>
#include <stdexcept>

using namespace bbcpp;

/* Internal wrapper structures */
struct bbcpp_document_t {
    BBDocumentPtr doc;

    bbcpp_document_t(BBDocumentPtr d) : doc(d) {}
};

struct bbcpp_node_t {
    BBNodePtr node;

    bbcpp_node_t(BBNodePtr n) : node(n) {}
};

/* Helper functions */
static bbcpp_error copy_string(const std::string& source, char* buffer, size_t buffer_size, size_t* length) {
    if (!buffer || !length) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    auto source_len = source.length();
    *length = source_len;

    if (buffer_size == 0) {
        return BBCPP_SUCCESS; // Just return the required length
    }

    if (buffer_size <= source.length()) {
        return BBCPP_ERROR_BUFFER_TOO_SMALL;
    }

    std::memcpy(buffer, source.c_str(), source_len);
    return BBCPP_SUCCESS;
}

static bbcpp_node_type convert_node_type(BBNode::NodeType type) {
    switch (type) {
        case BBNode::NodeType::DOCUMENT: return BBCPP_NODE_DOCUMENT;
        case BBNode::NodeType::ELEMENT: return BBCPP_NODE_ELEMENT;
        case BBNode::NodeType::TEXT: return BBCPP_NODE_TEXT;
        case BBNode::NodeType::ATTRIBUTE: return BBCPP_NODE_ATTRIBUTE;
        default: return BBCPP_NODE_TEXT;
    }
}

static bbcpp_element_type convert_element_type(BBElement::ElementType type) {
    switch (type) {
        case BBElement::SIMPLE: return BBCPP_ELEMENT_SIMPLE;
        case BBElement::VALUE: return BBCPP_ELEMENT_VALUE;
        case BBElement::PARAMETER: return BBCPP_ELEMENT_PARAMETER;
        case BBElement::CLOSING: return BBCPP_ELEMENT_CLOSING;
        default: return BBCPP_ELEMENT_SIMPLE;
    }
}

/* Document functions */
bbcpp_document_handle bbcpp_document_create(void) {
    try {
        auto doc = BBDocument::create();
        return new bbcpp_document_t(doc);
    } catch (...) {
        return nullptr;
    }
}

void bbcpp_document_destroy(bbcpp_document_handle doc) {
    if (doc) {
        delete doc;
    }
}

bbcpp_error bbcpp_document_load(bbcpp_document_handle doc, const char* bbcode) {
    if (!doc || !bbcode) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        doc->doc->load(std::string(bbcode));
        return BBCPP_SUCCESS;
    } catch (const std::exception&) {
        return BBCPP_ERROR_PARSE_ERROR;
    } catch (...) {
        return BBCPP_ERROR_PARSE_ERROR;
    }
}

bbcpp_error bbcpp_document_get_children_count(bbcpp_document_handle doc, size_t* count) {
    if (!doc || !count) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        *count = doc->doc->getChildren().size();
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_document_get_child(bbcpp_document_handle doc, size_t index, bbcpp_node_handle* node) {
    if (!doc || !node) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        const auto& children = doc->doc->getChildren();
        if (index >= children.size()) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        *node = new bbcpp_node_t(children[index]);
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_document_print(bbcpp_document_handle doc) {
    if (!doc) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        printDocument(*doc->doc);
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

/* Node functions */
bbcpp_error bbcpp_node_get_type(bbcpp_node_handle node, bbcpp_node_type* type) {
    if (!node || !type) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        *type = convert_node_type(node->node->getNodeType());
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_node_get_name(bbcpp_node_handle node, char* buffer, size_t buffer_size, size_t* name_length) {
    if (!node) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        return copy_string(node->node->getNodeName(), buffer, buffer_size, name_length);
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_node_get_children_count(bbcpp_node_handle node, size_t* count) {
    if (!node || !count) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        *count = node->node->getChildren().size();
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_node_get_child(bbcpp_node_handle node, size_t index, bbcpp_node_handle* child) {
    if (!node || !child) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        const auto& children = node->node->getChildren();
        if (index >= children.size()) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        *child = new bbcpp_node_t(children[index]);
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_node_get_parent(bbcpp_node_handle node, bbcpp_node_handle* parent) {
    if (!node || !parent) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        auto parent_ptr = node->node->getParent();
        if (!parent_ptr) {
            *parent = nullptr;
            return BBCPP_SUCCESS;
        }

        *parent = new bbcpp_node_t(parent_ptr);
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

/* Text node functions */
bbcpp_error bbcpp_text_get_content(bbcpp_node_handle node, char* buffer, size_t buffer_size, size_t* content_length) {
    if (!node) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        if (node->node->getNodeType() != BBNode::NodeType::TEXT) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto text_node = node->node->downCast<BBTextPtr>();
        if (!text_node) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        return copy_string(text_node->getText(), buffer, buffer_size, content_length);
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

/* Element node functions */
bbcpp_error bbcpp_element_get_type(bbcpp_node_handle node, bbcpp_element_type* type) {
    if (!node || !type) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        if (node->node->getNodeType() != BBNode::NodeType::ELEMENT) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto element_node = node->node->downCast<BBElementPtr>();
        if (!element_node) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        *type = convert_element_type(element_node->getElementType());
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_element_get_parameter_count(bbcpp_node_handle node, size_t* count) {
    if (!node || !count) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        if (node->node->getNodeType() != BBNode::NodeType::ELEMENT) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto element_node = node->node->downCast<BBElementPtr>();
        if (!element_node) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        *count = element_node->getParameters().size();
        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_element_get_parameter_by_index(bbcpp_node_handle node, size_t index,
                                                 char* key_buffer, size_t key_buffer_size, size_t* key_length,
                                                 char* value_buffer, size_t value_buffer_size, size_t* value_length) {
    if (!node) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        if (node->node->getNodeType() != BBNode::NodeType::ELEMENT) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto element_node = node->node->downCast<BBElementPtr>();
        if (!element_node) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        const auto& params = element_node->getParameters();
        if (index >= params.size()) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto it = params.begin();
        std::advance(it, index);

        bbcpp_error key_result = copy_string(it->first, key_buffer, key_buffer_size, key_length);
        bbcpp_error value_result = copy_string(it->second, value_buffer, value_buffer_size, value_length);

        if (key_result != BBCPP_SUCCESS) return key_result;
        return value_result;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_element_get_parameter(bbcpp_node_handle node, const char* key,
                                        char* value_buffer, size_t value_buffer_size, size_t* value_length) {
    if (!node || !key) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        if (node->node->getNodeType() != BBNode::NodeType::ELEMENT) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto element_node = node->node->downCast<BBElementPtr>();
        if (!element_node) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        try {
            std::string value = element_node->getParameter(std::string(key), true);
            return copy_string(value, value_buffer, value_buffer_size, value_length);
        } catch (const std::invalid_argument&) {
            return BBCPP_ERROR_NOT_FOUND;
        }
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

bbcpp_error bbcpp_element_has_parameter(bbcpp_node_handle node, const char* key, int* has_parameter) {
    if (!node || !key || !has_parameter) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        if (node->node->getNodeType() != BBNode::NodeType::ELEMENT) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        auto element_node = node->node->downCast<BBElementPtr>();
        if (!element_node) {
            return BBCPP_ERROR_INVALID_ARGUMENT;
        }

        try {
            element_node->getParameter(std::string(key), true);
            *has_parameter = 1;
        } catch (const std::invalid_argument&) {
            *has_parameter = 0;
        }

        return BBCPP_SUCCESS;
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

/* Utility functions */
bbcpp_error bbcpp_get_raw_string(bbcpp_node_handle node, char* buffer, size_t buffer_size, size_t* content_length) {
    if (!node) {
        return BBCPP_ERROR_NULL_POINTER;
    }

    try {
        std::string raw_string = getRawString(*node->node);
        return copy_string(raw_string, buffer, buffer_size, content_length);
    } catch (...) {
        return BBCPP_ERROR_INVALID_ARGUMENT;
    }
}

const char* bbcpp_error_string(bbcpp_error error) {
    switch (error) {
        case BBCPP_SUCCESS: return "Success";
        case BBCPP_ERROR_NULL_POINTER: return "Null pointer error";
        case BBCPP_ERROR_INVALID_ARGUMENT: return "Invalid argument";
        case BBCPP_ERROR_BUFFER_TOO_SMALL: return "Buffer too small";
        case BBCPP_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case BBCPP_ERROR_PARSE_ERROR: return "Parse error";
        case BBCPP_ERROR_NOT_FOUND: return "Not found";
        default: return "Unknown error";
    }
}
