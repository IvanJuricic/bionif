#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Fasta {
    private:
        std::string name, sequence;
    public:
        Fasta(std::string n, std::string s) {
            name = n;
            sequence = s;
        }

        void set_name(std::string n) { name = n; }
        void set_sequence(std::string s) { sequence = s; }
        void print_sequence() { std::cout << ">" << name << "\n" << sequence << "\n"; }
};

// Parse fasta file and return Fasta objects to a vector<Fasta> pointer
void fasta_parser(std::vector<Fasta> * sequences, std::string filename)
{
    std::ifstream infile(filename, std::ios::out | std::ios::in | std::ios::binary);
    if ( !infile.is_open() )
        std::cout<<"Could not open file\n";
    else {
        std::string line;
        std::string name;
        std::string sequence;

        while (std::getline(infile, line))
        {
            if (line[0] == '>')
            {
                name = line.erase(0, 1);
            }
            else
            {
                sequence = line;
                Fasta seq = Fasta(name, sequence);
                sequences -> push_back(seq);
            }
        }
    }
}

int main(void) {

    std::string genomeFilePath = "data";
    std::string line, name, content;

    std::cout << "Hello there" << std::endl;
    
    std::ifstream genome;
    genome.open(genomeFilePath);

    if(genome.is_open()) {
        while (std::getline(genome, line))
        {
            if( line.empty() || line[0] == '>' ){ // Identifier marker
                if( !name.empty() ){ // Print out what we read from the last entry
                    std::cout << name << " : " << content << std::endl;
                    name.clear();
                }
                if( !line.empty() ){
                    name = line.substr(1);
                }
                content.clear();
            } else if( !name.empty() ){
                if( line.find(' ') != std::string::npos ){ // Invalid sequence--no spaces allowed
                    name.clear();
                    content.clear();
                } else {
                    content += line;
                }
            }
            //std::cout << line << std::endl;
        }

        if( !name.empty() ){ // Print out what we read from the last entry
            std::cout << name << " : " << content << std::endl;
        }

        genome.close();
        
    } else {
        std::cout << "Unable to open requested file" << std::endl;
    }
    
   // Parse fasta file
    /*std::vector<Fasta> sequences;
    fasta_parser(&sequences, genomeFilePath);

    for (Fasta s : sequences)
    {
        s.print_sequence();
    }*/

    return 0;
}