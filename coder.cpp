#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sys/stat.h>
#include <stdexcept>

using namespace std;

int indexForSymbol(char c, vector<pair<char, unsigned int>> arr){
    for (int i = 0; i < arr.size(); i++)
    {
        if(c == arr[i].first){
            return i+2;
        }
    }

    return -1;
    
}

void output_bit(unsigned int bit, unsigned int* bit_len, unsigned char* write_bit, FILE* output_file){
    (*write_bit) >>= 1;
    if(bit) (*write_bit) |= 0x80;
    (*bit_len)--;
    if((*bit_len) == 0){
        fputc((*write_bit), output_file);
        (*bit_len) = 8;
    }
}

void bitsPlusFollow(unsigned int bit, unsigned int* bits_to_follow, unsigned int* bit_len, unsigned char* write_bit, FILE* output_file){
    output_bit(bit, bit_len, write_bit, output_file);

    for (; *bits_to_follow > 0; (*bits_to_follow)--){
        output_bit(!bit, bit_len, write_bit, output_file);
    }
}
double coder(const char* input_name="input.txt", const char* output_name="encoded.txt"){
    unsigned int * alfabet = new unsigned int [256];
    for (int i=0; i<256; i++){
        alfabet[i] = 0;
    }

    FILE* input_file = fopen(input_name, "rb"); 
    if (input_file == nullptr) {
       throw invalid_argument("File not found.");
    }

    unsigned long long coll_characters = 0;
    unsigned char character = 0;
    while (!feof(input_file)) { // Read from input file
       character = fgetc(input_file);
       if(!feof(input_file)){
           coll_characters++;
           alfabet[character]++;
       }
    }
    
    fclose(input_file);

    vector<pair<char, unsigned int>> arr;
    for(int i=0; i<256; i++){
        if(alfabet[i] != 0){
            arr.push_back(make_pair((char)i, alfabet[i]));
        }
    }

    sort(arr.begin(), arr.end(),
            [](const pair<char, unsigned int> &l, const pair<char, unsigned int> &r)
            {
                if (l.second != r.second) {
                    return l.second >= r.second;
                }
 
                return l.first < r.first;
            });


    unsigned long long* table = new unsigned long long[arr.size() + 2];
    table[0] = 0;
    table[1] = 1;
    for (int i = 0; i < arr.size(); i++){
        unsigned int b = arr[i].second;
        for (int j = 0; j<i; j++){
            b += arr[j].second;
        }
        table[i+2] = b;
    }

    if(table[arr.size()] > (1<<14 -1)){
        throw invalid_argument("Error, too long count.");
    }
    unsigned long long low_v = 0;
    unsigned long long high_v = (((unsigned long long)(1)<<16) - 1);
    unsigned long long delitel = table[arr.size()+1];
    unsigned long long diff = high_v - low_v + 1;
    unsigned long long first_qtr = (high_v + 1) / 4;
    unsigned long long half = first_qtr * 2;
    unsigned long long third_qtr = first_qtr * 3;
    unsigned int bits_to_follow = 0;
    unsigned int bit_len = 8;
    unsigned char write_bit = 0;
    int j = 0;
    
	input_file = fopen(input_name, "rb"); 
    FILE* output_file = fopen(output_name, "wb +");

    char col_letters = arr.size();
    fputc(col_letters, output_file);

    for(int i=0; i<256; i++){ // Writing the letters used and their number
        if(alfabet[i] != 0){
            fputc((char)i, output_file);
            fwrite(reinterpret_cast<const char*>(&alfabet[i]), sizeof(unsigned int), 1, output_file);
        }
    }

    int i = 0;
	while (!feof(input_file)) { // Read from input file

        character = fgetc(input_file);

        if(!feof(input_file)){

            j = indexForSymbol(character, arr);
            i++;

            high_v = low_v +  table[j] * diff / delitel - 1;
            low_v = low_v + table[j - 1] * diff  / delitel;
            if (low_v > high_v)
                {
                    cout<<"Hehehe: "<<low_v<<" "<<high_v<<endl;
                }
            for (;;){
                
                
                
                if (high_v < half){
                    bitsPlusFollow(0, &bits_to_follow, &bit_len, &write_bit, output_file);
                }

                else if (low_v >= half){
                    bitsPlusFollow(1, &bits_to_follow, &bit_len, &write_bit, output_file);
                    low_v -= half;
                    high_v -= half;
                }

                else if ((low_v >= first_qtr) && (high_v < third_qtr)){
                    bits_to_follow++;
                    low_v -= first_qtr;
                    high_v -= first_qtr;
                }
                else break;

                low_v += low_v;
                high_v += high_v + 1;


            }
        }
        else{
        
            high_v = low_v +  table[1] * diff / delitel - 1;
            low_v = low_v + table[0] * diff  / delitel;
        

            for (;;){
                
                
                if (high_v < half){
                    bitsPlusFollow(0, &bits_to_follow, &bit_len, &write_bit, output_file);
                }

                else if (low_v >= half){
                    bitsPlusFollow(1, &bits_to_follow, &bit_len, &write_bit, output_file);
                    low_v -= half;
                    high_v -= half;
                }

                else if ((low_v >= first_qtr) && (high_v < third_qtr)){
                    bits_to_follow++;
                    low_v -= first_qtr;
                    high_v -= first_qtr;
                }
                else break;

                low_v += low_v;
                high_v += high_v + 1;


            }
            bits_to_follow+=1;
            
            if(low_v < first_qtr){
                bitsPlusFollow(0, &bits_to_follow, &bit_len, &write_bit, output_file);
            }
            else{
                bitsPlusFollow(1, &bits_to_follow, &bit_len, &write_bit, output_file);
            }

            write_bit >>= bit_len;
            fputc(write_bit, output_file);
            
        }
        diff = high_v - low_v + 1;
	}


    fclose(input_file);
    fclose(output_file);

    unsigned long long file_full_size = 0;
    unsigned long long commpres_size = 0;
    struct stat sb{};
    struct stat se{};
    // Finding compression ratio
    if (!stat(input_name, &sb)) {
        file_full_size = sb.st_size;
    }
    else {
        perror("stat");
    }
    if (!stat(output_name, &se)) {
        commpres_size = se.st_size;
    }
    else {
        perror("stat");
    }
    return (commpres_size + 0.0 ) / file_full_size;
}

int main(){
    cout<<coder()<<endl; // Print compression ratio
}