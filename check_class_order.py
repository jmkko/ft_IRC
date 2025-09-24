#!/usr/bin/env python3
"""
Script pour vérifier que les sections public sont avant les sections private
dans les classes C++.
"""

import re
import sys
import os
from pathlib import Path

def check_class_order(file_content, filename):
    """Vérifie l'ordre des sections dans les classes C++."""
    errors = []
    
    # Regex pour trouver les classes et leurs sections
    class_pattern = r'class\s+(\w+)[^{]*\{([^}]*(?:\{[^}]*\}[^}]*)*)\}'
    access_pattern = r'(public|private|protected)\s*:'
    
    classes = re.finditer(class_pattern, file_content, re.MULTILINE | re.DOTALL)
    
    for class_match in classes:
        class_name = class_match.group(1)
        class_body = class_match.group(2)
        
        # Trouve toutes les sections d'accès
        access_specifiers = []
        for match in re.finditer(access_pattern, class_body):
            access_specifiers.append((match.group(1), match.start()))
        
        # Vérifie l'ordre
        if len(access_specifiers) >= 2:
            for i in range(len(access_specifiers) - 1):
                current = access_specifiers[i][0]
                next_spec = access_specifiers[i + 1][0]
                
                # Règle : public doit être avant private
                if current == 'private' and next_spec == 'public':
                    errors.append(f"{filename}:{class_name}: 'public' section found after 'private' section")
    
    return errors

def main():
    """Point d'entrée principal."""
    if len(sys.argv) < 2:
        print("Usage: python check_class_order.py <file1.cpp> [file2.cpp ...]")
        sys.exit(1)
    
    all_errors = []
    
    for filename in sys.argv[1:]:
        if not os.path.exists(filename):
            print(f"Erreur: {filename} n'existe pas")
            continue
            
        with open(filename, 'r', encoding='utf-8') as f:
            content = f.read()
            
        errors = check_class_order(content, filename)
        all_errors.extend(errors)
    
    if all_errors:
        print("Erreurs d'ordre des sections trouvées :")
        for error in all_errors:
            print(f"  {error}")
        sys.exit(1)
    else:
        print("Toutes les classes respectent l'ordre public → private ✓")

if __name__ == '__main__':
    main()