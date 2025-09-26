#!/usr/bin/env python3
"""
Simple script to check for unused includes in C++ header files.
This is a basic analysis - more sophisticated tools like include-what-you-use are better.
"""

import re
import sys
from pathlib import Path

def extract_includes(file_path):
    """Extract all #include statements from a file."""
    includes = []
    try:
        with open(file_path, 'r') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if line.startswith('#include'):
                    # Extract the included file
                    match = re.search(r'#include\s*[<"]([^>"]+)[>"]', line)
                    if match:
                        includes.append((match.group(1), line_num, line))
    except FileNotFoundError:
        print(f"File not found: {file_path}")
        return []
    return includes

def check_usage_in_file(file_path, include_name):
    """Check if symbols from an include are used in the file."""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
            
        # Remove comments and strings to avoid false positives
        content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)
        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
        content = re.sub(r'"[^"]*"', '', content)
        content = re.sub(r"'[^']*'", '', content)
        
        # Common patterns to check based on include name
        if include_name == 'map':
            return bool(re.search(r'\bstd::map\b|\bmap\s*<', content))
        elif include_name == 'vector':
            return bool(re.search(r'\bstd::vector\b|\bvector\s*<', content))
        elif include_name == 'string':
            return bool(re.search(r'\bstd::string\b|\bstring\b', content))
        elif include_name == 'fstream':
            return bool(re.search(r'\bstd::(?:ifstream|ofstream|fstream)\b|\b(?:ifstream|ofstream|fstream)\b', content))
        elif include_name == 'sstream':
            return bool(re.search(r'\bstd::(?:stringstream|ostringstream|istringstream)\b|\b(?:stringstream|ostringstream|istringstream)\b', content))
        elif include_name == 'iostream':
            return bool(re.search(r'\bstd::(?:cout|cin|cerr|clog)\b|\b(?:cout|cin|cerr|clog)\b', content))
        elif include_name == 'iomanip':
            return bool(re.search(r'\bstd::(?:setw|setfill|setprecision)\b|\b(?:setw|setfill|setprecision)\b', content))
        elif include_name == 'ctime':
            return bool(re.search(r'\bstd::(?:time|tm|localtime)\b|\b(?:time|tm|localtime)\b', content))
        else:
            # Generic check - look for the include name in the code
            base_name = include_name.split('/')[-1].replace('.hpp', '').replace('.h', '')
            return bool(re.search(rf'\b{base_name}\b', content))
            
    except FileNotFoundError:
        return False

def analyze_header_includes(header_path):
    """Analyze includes in a header file."""
    print(f"\n=== Analyzing {header_path} ===")
    
    includes = extract_includes(header_path)
    if not includes:
        print("No includes found or file not accessible.")
        return
        
    print(f"Found {len(includes)} includes:")
    
    unused_includes = []
    used_includes = []
    
    for include_name, line_num, full_line in includes:
        is_used = check_usage_in_file(header_path, include_name)
        
        if is_used:
            used_includes.append((include_name, line_num))
            print(f"  ✓ Line {line_num}: {full_line} - USED")
        else:
            unused_includes.append((include_name, line_num))
            print(f"  ✗ Line {line_num}: {full_line} - POTENTIALLY UNUSED")
    
    if unused_includes:
        print(f"\n⚠️  Potentially unused includes ({len(unused_includes)}):")
        for include_name, line_num in unused_includes:
            print(f"  - Line {line_num}: #include <{include_name}>")
    
    return unused_includes, used_includes

if __name__ == "__main__":
    if len(sys.argv) > 1:
        file_path = sys.argv[1]
    else:
        file_path = "includes/server/Logger.hpp"
    
    analyze_header_includes(file_path)