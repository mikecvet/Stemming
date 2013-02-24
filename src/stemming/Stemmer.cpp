#include "Stemmer.hpp"

using namespace gl;

Stemmer::Stemmer (const char *corpus_path)
{
  __load_corpus (corpus_path);
}

Stemmer::~Stemmer ()
{
}

void
Stemmer::__load_corpus (const char *path)
{
  int count;
  size_t pos;
  size_t total = 0;
  std::string line;
  std::string word;
  std::string countstr;
  std::ifstream f;

  f.open (path, std::ios::in);

  if (f.fail ())
    {
      std::cerr << "Failed to open corpus file" << std::endl;
      abort ();
    }

  while (!f.eof ())
    {
      std::getline (f, line);

      if (line.length ())
	{
	  pos = line.find (DELIMITER);
	  word.assign (line.substr (0, pos));
	  countstr.assign (line.substr (pos + 1, std::string::npos));
	  line.clear ();
	  count = atoi (countstr.c_str ());
	  
	  __word_counts[word].count = count;
	  total += count;
	}
    }

  std::cout << "Loaded " << __word_counts.size () << " words" << std::endl;

  double T = total;
  std::tr1::unordered_map<std::string, WordData>::iterator word_iter = __word_counts.begin ();
  std::tr1::unordered_map<std::string, WordData>::iterator word_end = __word_counts.end ();

  while (word_iter != word_end)
    {
      (word_iter->second).occ = (word_iter->second).count / T;
      ++word_iter;
    }
}

void
Stemmer::stem ()
{
  std::tr1::unordered_map<std::string, WordData>::const_iterator word_iter = __word_counts.begin ();
  std::tr1::unordered_map<std::string, WordData>::const_iterator word_end = __word_counts.end ();

  stemming::english_stem<> english_stemmer;

  while (word_iter != word_end)
    {
      const std::string &word = word_iter->first;
      ++word_iter;

      std::string stem (word.length (), ' ');
      std::wstring w (word.length (), L' ');
      std::copy (word.begin (), word.end (), w.begin ());
      
      english_stemmer (w); // w is now a stemmed word;
      
      std::copy (w.begin (), w.end (), stem.begin ());

      while (stem[stem.length () - 1] == ' ')
	stem.erase (stem.length () - 1);

      __inverse_stem_map[stem].push_back (word);
    }

  std::cout << "calculated " << __inverse_stem_map.size () << " stems." << std::endl;
}

void
Stemmer::unstem (const std::string &w, std::vector<UnstemData> &data)
{
  double min = 9999.0;
  double max = 0.0;
  std::tr1::unordered_map<std::string, std::vector<std::string> >::const_iterator iter = __inverse_stem_map.find (w);

  if (iter != __inverse_stem_map.end ())
    {
      __confidence ((iter->second), data);
    }
}

void
Stemmer::__confidence (const std::vector<std::string> &words, std::vector<UnstemData> &data)
{
  double min = 9999.0;
  double max = 0.0;
  size_t n = words.size ();

  std::vector<const WordData*> v;
  v.reserve (words.size ());

  for (int i = 0; i < n; i++)
    {
      const WordData &wd = __word_counts[words[i]];

      if (wd.occ > max)
	max = wd.occ;

      if (wd.occ < min)
	min = wd.occ;

      v.push_back (&wd);
    }

  for (int i = 0; i < n; i++)
    {
      const WordData &wd = *v[i]; //__word_counts[words[i]];

      UnstemData d;
      d.word = words[i];

      if (max - min > 0.0)
	d.confidence = (wd.occ - min) / (max - min);
      else
	d.confidence = 1.0;

      data.push_back (d);
    }
}

void
Stemmer::dump ()
{
  size_t n;
  std::tr1::unordered_map<std::string, std::vector<std::string> >::const_iterator iter = __inverse_stem_map.begin ();
  std::tr1::unordered_map<std::string, std::vector<std::string> >::const_iterator end = __inverse_stem_map.end ();

  while (iter != end)
    {
      std::vector<gl::UnstemData> data;
      
      unstem ((iter->first), data);

      std::cout << (iter->first) << " : ";
      
      n = data.size ();
      for (int j = 0; j < n; j++)
        {
	  std::cout << data[j].word << "/" << data[j].confidence << " ";
        }
      std::cout << std::endl;

      ++iter;
    }
}
