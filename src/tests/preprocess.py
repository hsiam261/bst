import sys
import re

def replace_include_with_file_contents(file_path):
    with open(file_path, 'r') as file:
        content = file.read()
    
    def include_replace(match):
        include_file_path = match.group(1)
        with open(include_file_path, 'r') as include_file:
            return include_file.read()
    
    replaced_content = re.sub(r'#include\s+"(.+?)"', include_replace, content)
    
    return replaced_content

# Check if the file path is provided as a command line argument
if len(sys.argv) < 2:
    print("Usage: python script.py <file_path>")
    sys.exit(1)

file_path = sys.argv[1]
new_content = replace_include_with_file_contents(file_path)
print(new_content)

