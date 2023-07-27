#!/usr/bin/env python3

# Copyright 2023 Mikael Lund aka Wombat
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the license at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the license is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the license for the specific language governing permissions and
# limitations under the license.

# This script extracts doxygen generated XML files and prints
# the output in a format that can be used in the MEGA65 LaTeX guide.
# Relevant XML files are name *8h.xml.

import xmltodict
import argparse
import glob
from dataclasses import dataclass

@dataclass
class FunctionRecord:
    """Class for storing Doxygen metadata"""
    name: str
    brief: str
    definition: str
    argsstring: str
    return_desc: str
    parameters: list
        
    def print_m65_guide(self):
        ''' Print in syntax used for the MEGA65 LaTeX guide '''
        print(f'\m65libsummary{{{self.name}}}{{{self.brief}}}')
        print(f'\m65libsyntax{{{self.definition}{self.argsstring}}}')
        for arg in self.parameters:
            print('\m65libparam{{{}}}{{{}}}'.format(arg['name'], arg['desc']))
        if isinstance(self.return_desc, str):
            print(f'\m65libretval{{{self.return_desc}}}')
            
def find_section(db):
    ''' Finds dictionary with function definitions '''
    if 'doxygen' not in db:
        return {}
    for key, val in db['doxygen']['compounddef'].items():
        if key != 'sectiondef':
            continue
        if isinstance(val, dict):
            return val['memberdef']
        if isinstance(val, list):
            for i in val:
                if i['@kind'] == 'func':
                    return i['memberdef']
    return {}

def extract_arguments(detail):
    ''' Extracts function arguments from detailed description'''
    if detail is None:
        return []
    if not isinstance(detail['para'], dict):
        return []
    if 'parameterlist' not in detail['para']:
        return []
    parameters = []
    for p in detail['para']['parameterlist']['parameteritem']:
        if isinstance(p, dict):
            name = p['parameternamelist']['parametername']
            desc = p['parameterdescription']['para']
            parameters.append(dict(name=name, desc=desc))
    return parameters

def extract_return(detail):
    ''' Extracts return value from detailed description'''
    if detail is None:
        return None
    if not isinstance(detail['para'], dict):
        return None
    if 'simplesect' in detail['para']:
        d = detail['para']['simplesect']
        if isinstance(d, dict):
            return d['para']
        if isinstance(d, list):
            for val in d:
                if val['@kind'] == 'return':
                    return val['para']
    return None

def extract_brief(i):
    if not isinstance(i, dict):
        return None
    if i['@kind'] != 'function':
        return None
    return i['briefdescription']

def parse_function(i):
    ''' Parses a single element and return a dictionary if a function is found '''
    brief = extract_brief(i)
    if brief is None:
        return
    name = i['name']
    argsstring = i['argsstring']
    definition = i['definition']
    detail = i['detaileddescription']
    
    # Handle function arguments
    return_desc = extract_return(detail)
    parameters = extract_arguments(detail)

    return FunctionRecord(name = name, definition = definition, brief = brief,
                               parameters = parameters, argsstring = argsstring,
                               return_desc = return_desc)

def parse_xml(file):
    '''Parses a single XML file and prints the output'''
    db = xmltodict.parse(file.read())
    members = find_section(db)
    for i in members:
        func = parse_function(i)
        if func is not None:
            func.print_m65_guide()
            print()

def match_files(files):
    '''Match files with glob patterns'''
    matched_files = []
    for file in files:
        if glob.escape(file) != file:
            # -> There are glob pattern chars in the string
            matched_files.extend(glob.glob(file))
        else:
            matched_files.append(file)
    return matched_files

def main():
    parser = argparse.ArgumentParser(
            prog='m65guide.py',
            description='Extract doxygen generated XML to m65lib documentation')
    parser.add_argument('files', nargs='+',
                        help='input xml files - should end with *8h.xml')           # positional argument
    args = parser.parse_args()

    for filename in match_files(args.files):
        with open(filename) as f:
            parse_xml(f)

if __name__ == "__main__":
    main()
