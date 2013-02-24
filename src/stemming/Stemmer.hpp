#ifndef STEMMER_HPP_
#define STEMMER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <tr1/unordered_map>
#include <vector>

#include "../oleander/english_stem.h"

namespace gl
{
  const std::string DELIMITER (",");

  typedef struct
  {
    int count;
    double occ;
  } WordData;

  typedef struct
  {
    double confidence;
    std::string word;
  } UnstemData;

  class Stemmer
  {
  private:
    std::tr1::unordered_map<std::string, WordData> __word_counts;
    std::tr1::unordered_map<std::string, std::vector<std::string> > __inverse_stem_map;

    void __load_corpus (const char *path);
    void __confidence (const std::vector<std::string> &words, std::vector<UnstemData> &data);
    
  public:
    Stemmer (const char *corpus_path);
    ~Stemmer ();

    void stem ();
    void dump ();
    void unstem (const std::string &w, std::vector<UnstemData> &data);
  };
}

#endif /* STEMMER_HPP_ */
