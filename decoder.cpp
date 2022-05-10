#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <stdexcept>

using namespace std;

int input_bit (unsigned char* read_bit, unsigned int* bit_len, FILE* input_file, unsigned int* garbage_bit){
    if((*bit_len) == 0){
        (*read_bit) = fgetc(input_file); 
        if(feof(input_file)){
            (*garbage_bit)++;
            if((*garbage_bit)>14){
                throw invalid_argument("Can't decompress");
            }
            // return -1;
        }
        (*bit_len) = 8;
    }
    int t = (*read_bit) & 1;
    (*read_bit) >>= 1;
    (*bit_len)--;
    return t; 
}

int indexForSymbol (char c, vector<pair<char, unsigned int>> arr){
    for (int i = 0; i < arr.size(); i++)
    {
        if(c == arr[i].first){
            return i+1;
        }
    }

    return -1;
    
}

void decoder (const char* input_name="encoded.txt", const char* output_name="output.txt"){ // Decoding function
    unsigned int * alfabet = new unsigned int [256];
    for(int i=0; i<256; i++){
        alfabet[i] = 0;
    }
    FILE* input_file = fopen(input_name, "rb"); // Open input file
    if (input_file == nullptr) {
       throw invalid_argument("File not found.");
    }
    unsigned char col = 0;
    unsigned int col_letters = 0;
    col = fgetc(input_file);
    if(!feof(input_file)){
       col_letters =(unsigned int) col;
    }

    unsigned char character = 0;
    for(int i = 0; i< col_letters; i++){ // Reading the letters used and their number
        character = fgetc(input_file);
        if(!feof(input_file)){
            fread(reinterpret_cast<char*>(&alfabet[character]), sizeof(unsigned int), 1, input_file);
        }
        else{
            throw invalid_argument("Can't decompress file.");
        }
    }
    unsigned long long coll_characters = 0;
    vector<pair<char, unsigned int>> arr;
    for(int i=0; i<256; i++){
        if(alfabet[i] != 0){
            coll_characters += alfabet[i];
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

    
    unsigned long long low_v = 0;
    unsigned long long high_v = (((unsigned long long)(1)<<16) - 1);
    unsigned long long delitel = table[arr.size()+1];
    unsigned long long first_qtr = (high_v + 1) / 4;
    unsigned long long half = first_qtr * 2;
    unsigned long long third_qtr = first_qtr * 3;
    FILE* output_file = fopen(output_name, "wb +");
    unsigned int bit_len = 0;
    unsigned char read_bit = 0;
    unsigned int garbage_bit = 0;
    unsigned short value = 0;
    int k = 0;

    for(int i = 1; i <= 16; i++){
        k = input_bit(&read_bit, &bit_len, input_file, &garbage_bit);
        value = 2 * value + k;
    }
    unsigned long long diff = high_v - low_v + 1;
    for (;;) { // Read from input file

        

            
        unsigned long long freq = (unsigned long long )((((unsigned long long )value - low_v + 1) * delitel - 1) / (diff));
        
        int j;
        for( j = 1; table[j] <= freq; j++);
        high_v = low_v +  table[j] * diff / delitel - 1;
        low_v = low_v + table[j - 1] * diff  / delitel;
        if (high_v < low_v)
        {
            cout<<"HAHAHAHAHAHAH"<<endl;
        }
        for (;;){
                 
            if (high_v < half){
                
            }

            else if (low_v >= half){
                low_v -= half;
                high_v -= half;
                value -= half;
            }

            else if ((low_v >= first_qtr) && (high_v < third_qtr)){
                low_v -= first_qtr;
                high_v -= first_qtr;
                value -= first_qtr;
            }
            else break;
            
            low_v += low_v;
            high_v += high_v + 1;
            k = 0;
            k = input_bit(&read_bit, &bit_len, input_file, &garbage_bit);
            value += value + k;

            

        }
        
        if(j==1){
            break;
        }

        fputc(arr[j-2].first, output_file);
        diff = high_v - low_v + 1;
        
        
    }
    fclose(input_file);
    fclose(output_file);
}


unsigned int checker(const char* before_name="input.txt", const char* after_name="output.txt"){ // Checking for file matches 
    unsigned int same = 0;
    FILE* before_file = fopen(before_name, "r");
    FILE* after_file = fopen(after_name, "r");

    unsigned char after_l = 0;
    unsigned char before_l = 0;
    while (!feof(after_file) && !feof(before_file)) {
        after_l = fgetc(after_file);
        before_l = fgetc(before_file);
        if(!feof(after_file) && !feof(before_file)){
            if(after_l != before_l){
                same++;
            }
        }
    }

    while (!feof(after_file))
    {
        after_l = fgetc(after_file);
        if(!feof(after_file)){
            same++;
        }
    }

    while (!feof(before_file))
    {
        before_l = fgetc(before_file);
        if(!feof(before_file)){
            same++;
        }
    }
    fclose(after_file);
    fclose(before_file);
    return same;
}

int main(){
    decoder();
    cout<<checker()<<endl;
}