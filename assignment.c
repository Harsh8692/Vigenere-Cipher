// Author :  Harsh 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAX_LENGTH 5000

// Array to store English letter frequencies
float charfreq[26] = {8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966, 0.153, 0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987, 6.327, 9.056, 2.758, 0.978, 2.360, 0.150, 1.974, 0.074};

// Function to determine the rotation amount required for a Caesar cipher
// to convert the given text to ciphertext
int keyOfCeaser(char *s, int leng){
    float charfreq2[26] = {0};

    for(int i = 0; i < leng; i++){
        charfreq2[s[i] - 'A'] += 1.0;
    }

    // Converting frequencies to percentages
    for(int i = 0; i < 26; i++){
        charfreq2[i] *= 100.0;
        charfreq2[i] /= leng;
    }

    // Finding the rotation amount that maximizes the dot product between
    // the frequencies of characters in the input text and English letter frequencies
    int ans = 0; // Rotation amount
    float dotpdtmax = 0.0; // Maximum dot product
    for(int i = 0; i < 26; i++){
        float dotpdt = 0.0;
        for(int j = 0; j < 26; j++){
            dotpdt += charfreq2[(i+j)%26] * charfreq[j];
        }
        if(dotpdt > dotpdtmax){
            ans = i;
            dotpdtmax = dotpdt;
        }
    }
    return ans;
}

// Function to calculate the highest common factor (HCF) of two numbers using Euclid's algorithm
int hcf(int a, int b){
    if(b == 0)
        return a;
    return hcf(b, a % b);
}

// Function to return the maximum of two numbers
int max(int a, int b){
    if(a >= b)
        return a;
    return b;
}

// Function to return the minimum of two numbers
int min(int a, int b){
    if(a <= b)
        return a;
    return b;
}

// Function to decrypt a Caesar cipher with a given rotation
void decryptCeaser(char *str, int leng, int rotation){
    for(int i = 0; i < leng; i++){
        if(str[i] <= 'Z' && str[i] >= 'A') 
            str[i] = (str[i] - 'A' + rotation) % 26 + 'A'; // Decrypt the character
    }
}

// Function to calculate the summation of frequencies squared for a decrypted text corresponding to a given key length
float summationFreqSquareOfStrGivenKeyLeng(char *str, int leng, int keyleng){
    int* rotation;
    rotation = (int*)malloc(keyleng * sizeof(int));
    // Calculate rotations for each subtext
    for(int i = 0; i < keyleng; i++){
        char* temp2;
        temp2 = (char*)malloc(leng * sizeof(char));
        int idx2 = 0;
        for(int j = i; j < leng; j += keyleng){
            temp2[idx2] = str[j];
            idx2++;
        }
        temp2[idx2] = '\0';
        int rt = keyOfCeaser(temp2, idx2); // Find rotation for subtext
        rotation[i] = rt;
    }
    float freq[26] = {0};
    // Calculate frequencies for each subtext
    for(int i = 0; i < leng; i += keyleng){
        for(int j = 0; j < keyleng; j++){
            if((i + j) < leng){
                freq[(str[i + j] - 'A' + 26 - rotation[j]) % 26] += 1.0;
            }
        }
    }
    // Convert frequencies to percentages
    for(int i = 0; i < 26; i++){
        freq[i] /= leng;
    }
    float freqsq = 0.0;
    for(int i = 0; i < 26; i++){
        freqsq += freq[i] * freq[i];
    }
    free(rotation);
    rotation = NULL;
    return freqsq;
}

// Kasiski examination for trigram
// This function returns a value. All possible divisors of that value can be the correct key length that needs to be checked for the correct one.
int KasiskiExamination(char* str, int leng){
    int lengths[5000] = {0}; // Array to store frequencies of spacings between trigrams
    // Calculate frequencies of spacings between trigrams
    for(int i = 0; i < leng; i++){
        for(int j = i + 3; j < leng - 2; j++){
            if(str[i] == str[j] && str[i + 1] == str[j + 1] && str[i + 2] == str[j + 2]){
                lengths[j - i] += 1;
            }
        }
    }
    int mostfreq[10] = {0}; // Array to store top 10 most frequent spacings
    // Find top 10 most frequent spacings
    for(int i = 0; i < 10; i++){
        int id = 1;
        for(int j = 1; j < 5000; j++){
            if(lengths[j] >= lengths[id]){
                id = j;
            }
        }
        if(lengths[id]) // If the frequency is non-zero, store the spacing
            mostfreq[i] = id;
        lengths[id] = 0; // Set the frequency of the spacing to 0
    }

    int ans = hcf(max(mostfreq[0], mostfreq[1]), min(mostfreq[0], mostfreq[1])); // Initialize answer using the first two spacings
    // Update answer using the remaining spacings
    for(int i = 2; i < 10; i++){
        ans = hcf(max(ans, mostfreq[i]), min(ans, mostfreq[i]));
    }

    return ans; // Return the calculated key length
}

// Absolute function for floating-point values
float absf(float val){
    if(val < 0)
        return -val;
    return val;
}

// Function for decrypting Vigenere Cipher text
void decryptVigenere(char *str, int leng, char* originaltext, int originalLeng){
    // Determine possible key lengths using Kasiski Examination
    int kasiskileng = KasiskiExamination(str, leng);
    printf("Possible values of key length are divisors of %d\n", kasiskileng);

    float mn = 10.0; // Initialize minimum difference
    int correspleng = 1;

    // Loop through possible divisors of kasiskileng
    for(int i = 1; i * i <= kasiskileng; i++){
        if(kasiskileng % i == 0){
            
            float val = summationFreqSquareOfStrGivenKeyLeng(str, leng, i);
            printf("Value of summation of frequency square of decrypted text is %f for key length %d\n", val, i);
            
            if(absf(val - 0.068) < mn){
                mn = absf(val - 0.068);
                correspleng = i;
            }

            if(kasiskileng / i != i){
                float val = summationFreqSquareOfStrGivenKeyLeng(str, leng, kasiskileng / i);
                printf("Value of summation of frequency square of decrypted text is %f for key length %d\n", val, kasiskileng / i);
                
                if(absf(val - 0.068) < mn){
                    mn = absf(val - 0.068);
                    correspleng = kasiskileng / i;
                }
            }
        }
    }

    printf("We have value of summation of frequency square of decrypted text closest to 0.068 corresponding to length %d of keyword.\nSo the key used for encryption is of length %d\n", correspleng, correspleng);

    char* key;
    key = (char*) malloc((correspleng + 1) * sizeof(char));
    for(int i = 0; i < correspleng; i++){
        char * temp2;
        temp2 = (char* )malloc(leng * sizeof(char));
        int idx2 = 0;
        for(int j = i; j < leng; j += correspleng){
            temp2[idx2] = str[j];
            idx2++;
        }
        temp2[idx2] = '\0';
        int rotation = keyOfCeaser(temp2, idx2);
        key[i] = rotation + 'A'; // Store the rotation as a character in the key
    }
    key[correspleng] = '\0';
    printf("Key used for encryption is: %s\n", key);

    // Decrypt the text using the determined key
    printf("The decrypted text is: \n");
    for(int i = 0; i < originalLeng; i += correspleng){
        for(int j = 0; j < correspleng;){
            if((i + j) < originalLeng){
                if(originaltext[i + j] <= 'Z' && originaltext[i + j] >= 'A'){
                    // Decrypt the character and print it
                    printf("%c", (originaltext[i + j] - 'A' + 26 - (key[j] - 'A')) % 26 + 'A');
                    j++;
                }
                else{
                    // Print non-alphabetic characters without decryption and advance the index
                    printf("%c", originaltext[i + j]);
                    i++;
                }
            }
            else{
                j++;
            }
        }
    }
    printf("\n");
    free(key);
    key = NULL;
}


int main(){
    FILE* file;
    file = fopen("ciphertext.txt","r"); // Open input file in read mode
    freopen("output.txt","w",stdout); // Redirect standard output to output.txt
    char line[MAX_LENGTH]; // Array to store each line of the file
    char* str = NULL; // Pointer to dynamically allocated memory for the entire text
    size_t str_len = 0;

    // Read each line of the file and concatenate it to form the entire text
    while(fgets(line, MAX_LENGTH, file) != NULL){
        size_t line_len = strlen(line);
        str = (char*)realloc(str, str_len + line_len + 1); // Resize the memory block for the text
        if(str == NULL){ 
            printf("Memory allocation error!\n");
            return 1;
        }
        strcat(str, line); // Concatenate the line to the text
        str_len += line_len; // Update the length of the text
    }
    fclose(file); 

    int len;
    len = strlen(str); 
    char *temp1; 
    temp1 = (char *) malloc(len * sizeof(char)); 
    if(temp1 == NULL){ 
        printf("Memory allocation failed!\n");
        return 1;
    }
    int idx1 = 0; // Index for temp1
    for(int i = 0; i < len; i++){
        if(str[i] <= 'Z' && str[i] >= 'A'){ // If the character is an uppercase letter
            temp1[idx1] = str[i]; // Copy the character to temp1
            idx1++; // Increment the index
        }
        if(str[i] <= 'z' && str[i] >= 'a'){ // If the character is a lowercase letter
            str[i] = str[i] - 'a' + 'A'; // Convert the character to uppercase
            temp1[idx1] = str[i]; // Copy the character to temp1
            idx1++; // Increment the index
        }
    }
    temp1[idx1] = '\0';

    decryptVigenere(temp1, idx1, str, len); // Decrypt the Vigenere cipher text
    return 0;
}