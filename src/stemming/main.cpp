#include <iostream>
#include "Stemmer.hpp"

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      std::cerr << "usage: stemmer <corpus_path> ['dump'] | [stem1] [stem2] ..." << std::endl;
      return 1;
    }

  gl::Stemmer s (argv[1]);
  s.stem ();

  if (argc > 2 && !(strncmp (argv[2], "dump", 4)))
    {
      s.dump ();
    }
  else
    {
      for (int i = 2; i < argc; i++)
	{
	  std::string w (argv[i]);
	  std::vector<gl::UnstemData> data;
	  
	  s.unstem (w, data);
	  
	  size_t n = data.size ();
	  
	  std::cout << "Stem: " << w << "\n\t -> ";
	  for (int j = 0; j < n; j++)
	    {
	      std::cout << data[j].word << "/" << data[j].confidence << " ";
	    }
	  std::cout << std::endl;
	}
    }

  return 0;
}
