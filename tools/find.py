#!/usr/bin/env python3

import os
import re
from common import setup_common as setup

#         012345678901234567890123456
#phrase = "???a??i???n??l?u????i???u??"
phrase =  "oneadditionallaunchdiscount"
#         ???additionall?u???discount
#         establish?n??l?u???discount


def findFirst(dictionary, offset):
    list = []
    for word in dictionary:
        match = True
        isValid = 1
        if len(word) < 4:
            continue
        if len(word) > len(phrase) - offset:
            continue
        section = phrase[offset:offset+len(word)]
        for i, w in enumerate(word):
            if section[i] == '?':
                continue
            isValid -= 1
            if section[i] != word[i]:
                match = False
        if match and isValid <= 0:
            #list.append([word,section])
            list.append(word)
    return list

def makePhrase(dictionary, i,sentence,sin):
    list = findFirst(dictionary,i)
    #if len(list) == 0 and len(sin)-1 == len(phrase):
    if len(list) == 0 and len(sentence) == len(phrase):
        print(sin)
        return
    for match in list:
        #if len(phrase) > i + len(match) and phrase[i + len(match)] != '?':
        #    continue
       # makePhrase(dictionary, i + len(match) +1,sentence+match,sin+match+" ");
        makePhrase(dictionary, i + len(match),sentence+match,sin+match+" ");


def read_dict(path):
    if not os.path.isfile(path):
        raise FileNotFoundError('CSV File not found')

    file = open(path, mode="r")
    content = file.read()
    file.close()
    return content.splitlines()

project_root = setup.ROOT

def main():
    dictionary = read_dict(project_root / 'tools' / "dictionary.txt")
    makePhrase(dictionary, 0,"","")
    #print(findFirst(dictionary,6))

if __name__ == "__main__" :
    main()
