#pragma once

#include "BBDocument.h"

namespace bbcpp
{

// Helper Functions
std::string nodeTypeToString(BBNode::NodeType type);
std::string getIndentString(const unsigned int indent);
void printChildren(const BBNode& parent, unsigned int indent);
void printDocument(const BBDocument& doc);
std::string getRawString(const BBNode& node);

}