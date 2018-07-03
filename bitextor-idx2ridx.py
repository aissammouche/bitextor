#!__ENV__ __PYTHON__

#
# 1. Output from bitextor-lett2idx is read and an IDX file is obtained with an index of all the words in both languages and the list of documents where they occur
# 2. Words in both sides are translated by using the bilingual lexicon
# 3. Similarity metric based on bag-of-word-overlapping is computed
# 4. n-best documents are obtained for each document in the website
#
# Output format (RIDX):
# doc_id    [doc_id:score]+
#

import sys
import argparse
from sets import Set
from collections import defaultdict
from operator import itemgetter
import re

def readLETT(f, docs):
  file = open(f, "r")
  fileid = 1
  for i in file:
    fields = i.strip().split("\t")
    if len(fields) >= 5:
      #To compute the edit distance at the level of characters, HTML tags must be encoded as characters and not strings:
      docs[fileid] = fields[3]
    fileid += 1
  file.close()

#
# Building word indexes as dict files in python for both languages from the input IDX file
#
def rellenarIndex(file, lang1, lang2, index1, index2):
  for i in file:
    campos = i.strip().split("\t")
    if len(campos) == 3:
      if campos[0] == lang1 or campos[0] == lang2:
        documentos = campos[2].split(":")
        acum = 1

        for j in documentos:
          acum += int(j)
          if campos[0] == lang1:
            index1[acum].add(campos[1])
          else:
            index2[acum].add(campos[1])
  file.close()

#
# Loading bilingual lexicon (.dic)
#
def cargarDiccionarios(diccionario, lang1, lang2, dic):
  col_dic1 = -1
  col_dic2 = -1
  file = open(diccionario, "r")
  campos = file.readline().strip().split("\t")
  ind = 0
  for j in campos:
    if j == lang1:
      col_dic1 = ind
    elif j == lang2:
      col_dic2 = ind
    ind += 1
  for i in file:
    campos = i.strip().split("\t")
    if len(campos) == 2:
      dic[campos[col_dic2]].append(campos[col_dic1])
  file.close()

#
# Function that provides the set of translated words in a segments using a bilingual lexicon
#
def traducirPalabras(index, dic, dictp, translatedindex):
  for i in index:
    translatedindex[i] = Set([])
    contador = 0
    for word in index[i]:
      if word in dic:
        contador += 1
        translatedindex[i].update(dic[word])
    dictp[i] = contador

#
# The initial lexicon is extended by adding all those words that appear exactly the same in
# both sides (they are likely to be proper nouns, codes, dates, etc. that do not need to be translated).
#
def feedDictWithIdenticalWords(index1, index2, dic):
  words_lang1=Set()
  for key, words in index1.items():
    words_lang1=words_lang1.union(words)

  words_lang2=Set()
  for key, words in index2.items():
    words_lang2=words_lang2.union(words)

  for w in words_lang1.intersection(words_lang2):
    dic[w].append(w)

oparser = argparse.ArgumentParser(description="Script that reads the output of bitextor-lett2idx and builds an RIDX file (a list of documents and their corresponding n-best canidates to be parallel). To do so, a bag-of-word-overlapping metric is used to compare documents in both languages")
oparser.add_argument('idx', metavar='FILE', nargs='?', help='File produced by bitextor-lett2idx containing an index of the different words for every language in the website and the list of documents in which they appear (if undefined, the script will read from the standard input)', default=None)
oparser.add_argument('-d', help='Dictionary containing translations of words for the languages of the website; it is used to compute the overlapping scores which allow to relate documents in both languages)', dest="diccionario", required=True)
oparser.add_argument('-l', help='LETT file; if it is provided, document pair candidates are provided only if they belong to the same domain', dest="lett", required=False, default=None)
oparser.add_argument("--lang1", help="Two-characters-code for language 1 in the pair of languages", dest="lang1", required=True)
oparser.add_argument("--lang2", help="Two-characters-code for language 2 in the pair of languages", dest="lang2", required=True)
options = oparser.parse_args()

index_text1 = defaultdict(set)
index_text2 = defaultdict(set)
dic = defaultdict(list)
lista_palabras = []
encontrados = {}
dict_palabras = {}
translated_index_text2 = {}

#Loading bilingual lexicon
cargarDiccionarios(options.diccionario, options.lang1, options.lang2, dic)

if options.idx == None:
  reader = sys.stdin;
else:
  reader = open(options.idx, "r")

#Loading IDX file
rellenarIndex(reader, options.lang1, options.lang2, index_text1, index_text2)

#Extending the lexicon with words that are identical in both sides
feedDictWithIdenticalWords(index_text1, index_text2, dic)

#Translating all the words in the segments in language 2 into language 1 using the bilingual lexicon
traducirPalabras(index_text2, dic, dict_palabras, translated_index_text2)

if options.lett != None:
  documents = {}
  readLETT(options.lett, documents)

for i in index_text1:
  if options.lett != None:
    rx = re.match('(https?://)([^/]+)([^\?]*)(\?.*)?', documents[i])
    ihost = rx.group(2)

  parecidos = {}
  for j in index_text2:
    validpair = True
    if options.lett != None:
      rx = re.match('(https?://)([^/]+)([^\?]*)(\?.*)?', documents[j])
      jhost = rx.group(2)
      if jhost != ihost:
        validpair = False
    if validpair:
      c3 = index_text1[i].intersection(translated_index_text2[j])
      if len(c3) > 0 and int(dict_palabras[j]) > 0:
        max_vocab=max(len(index_text1[i]),len(index_text2[j]))
        min_vocab=min(len(index_text1[i]),len(index_text2[j]))
        num_intersect_words=len(c3)
        num_trans_words_text2=dict_palabras[j]
        parecidos[j] = (float(min_vocab)/float(max_vocab))*(float(num_intersect_words)/float(num_trans_words_text2))

  if len(parecidos) > 0:
    parecidos = sorted(parecidos.items(), key=itemgetter(1), reverse=True)
  encontrados[i] = []
  for j in parecidos:
    encontrados[i].append(str(j[0]) + ":" + str(j[1]))

# For each document, we obtain the 10-best candidates with highest score.
for i in encontrados:
  if len(encontrados[i]) > 10:
    contador = 10
  else:
    contador = len(encontrados[i])
  primera = True
  cadena = str(i) + "\t"
  for j in range(contador):
    if primera == True:
      cadena += str(encontrados[i][j])
      primera = False
    else:
      cadena += "\t" + str(encontrados[i][j])
  print cadena
