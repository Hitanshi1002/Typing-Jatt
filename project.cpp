#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <vector>
#include <limits>
#include <windows.h> // For color functionality on Windows
using namespace std;


// Function to set the console text color
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}


// Linked list node for text lines
struct LineNode {
    string text;
    LineNode* next;
    LineNode(const string& txt) : text(txt), next(nullptr) {}
};


// Graph class for word relationships
class WordGraph {
public:
    void addEdge(const string& word1, const string& word2) {
        adjacencyList[word1].push_back(word2);
        adjacencyList[word2].push_back(word1);
    }


    void displayConnections(const string& word) const {
        auto it = adjacencyList.find(word);
        if (it != adjacencyList.end()) {
            cout << "Words connected to \"" << word << "\": ";
            for (const auto& neighbor : it->second) {
                cout << neighbor << " ";
            }
            cout << "\n";
        } else {
            cout << "No connections found for \"" << word << "\".\n";
        }
    }


private:
    unordered_map<string, vector<string>> adjacencyList;
};


// TextEditor class with advanced data structures
class TextEditor {
public:
    TextEditor() {
        loadDefaultDictionary();
        loadCommonMisspellings();
        buffer = nullptr;
    }


    ~TextEditor() {
        clearBuffer();
    }


    void insertText() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter text to insert: ";
        string text;
        getline(cin, text);
        appendLine(text);
        checkSpellingInText(text);
        saveState();  // Save state for undo
    }


    void displayText() const {
        if (!buffer) {
            cout << "\nCurrent Text:\n[Empty]\n";
            return;
        }
        cout << "\nCurrent Text:\n";
        LineNode* current = buffer;
        while (current) {
            cout << current->text << "\n";
            current = current->next;
        }
    }


    void undo() {
        if (undoStack.empty()) {
            cout << "No actions to undo.\n";
            return;
        }
        buffer = undoStack.top();
        undoStack.pop();
        cout << "Undo successful.\n";
    }


    void addWordRelationship() {
        cout << "Enter the first word: ";
        string word1, word2;
        cin >> word1;
        cout << "Enter the second word: ";
        cin >> word2;
        wordGraph.addEdge(word1, word2);
        cout << "Relationship added between \"" << word1 << "\" and \"" << word2 << "\".\n";
    }


    void displayWordConnections() {
        cout << "Enter a word to see its connections: ";
        string word;
        cin >> word;
        wordGraph.displayConnections(word);
    }


    void searchWord() const {
        cout << "Enter the word to search: ";
        string targetWord;
        cin >> targetWord;


        int count = 0;
        LineNode* current = buffer;
        while (current) {
            if (containsWord(current->text, targetWord)) {
                count++;
                cout << "Found in line: " << current->text << "\n";
            }
            current = current->next;
        }


        if (count == 0) {
            cout << "Word \"" << targetWord << "\" not found in the text.\n";
        } else {
            cout << "Word \"" << targetWord << "\" found " << count << " time(s).\n";
        }
    }


    void replaceWord() {
        cout << "Enter the word to replace: ";
        string targetWord;
        cin >> targetWord;


        cout << "Enter the new word: ";
        string newWord;
        cin >> newWord;


        bool replaced = false;
        LineNode* current = buffer;
        while (current) {
            if (containsWord(current->text, targetWord)) {
                replaceInLine(current->text, targetWord, newWord);
                replaced = true;
            }
            current = current->next;
        }


        if (replaced) {
            cout << "Replaced all occurrences of \"" << targetWord << "\" with \"" << newWord << "\".\n";
            saveState();
        } else {
            cout << "Word \"" << targetWord << "\" not found in the text.\n";
        }
    }


  void ignoreWord() {
        cout << "Enter the word you want to ignore: ";
        string word;
        cin >> word;
        ignoredWords.insert(word);  // Add the word to the ignored set
        cout << "The word \"" << word << "\" will be ignored in future spell checks.\n";
    }


    void addToPersonalDictionary() {
        cout << "Enter the word to add to your personal dictionary: ";
        string word;
        cin >> word;
        dictionary.insert(word);  // Add the word to the dictionary
        cout << "The word \"" << word << "\" has been added to your personal dictionary.\n";
    }


    void run() {
        string command;
        while (true) {
            displayMenu();
            cout << "> ";
            cin >> command;
            if (command == "insert") {
                insertText();
            } else if (command == "display") {
                displayText();
            } else if (command == "undo") {
                undo();
            } else if (command == "addrel") {
                addWordRelationship();
            } else if (command == "connections") {
                displayWordConnections();
            } else if (command == "search") {
                searchWord();
            } else if (command == "replace") {
                replaceWord();
            } else if (command == "ignore") {
                ignoreWord();  // Add word to ignored list
            } else if (command == "adddict") {
                addToPersonalDictionary();  // Add word to dictionary
            } else if (command == "exit") {
                cout << "Exiting the text editor. Goodbye!\n";
                break;
            } else {
                cout << "Invalid command. Please try again.\n";
            }
        }
    }


private:
       LineNode* buffer;
    unordered_set<string> dictionary;
    unordered_set<string> ignoredWords;  // Set to store ignored words
    unordered_map<string, string> commonMisspellings;
    WordGraph wordGraph;
    stack<LineNode*> undoStack;


    bool containsWord(const string& line, const string& word) const {
        size_t pos = line.find(word);
        while (pos != string::npos) {
            bool isWholeWord = (pos == 0 || !isalnum(line[pos - 1])) &&
                               (pos + word.length() == line.length() || !isalnum(line[pos + word.length()]));
            if (isWholeWord) return true;
            pos = line.find(word, pos + 1);
        }
        return false;
    }


    void replaceInLine(string& line, const string& targetWord, const string& newWord) {
        size_t pos = line.find(targetWord);
        while (pos != string::npos) {
            bool isWholeWord = (pos == 0 || !isalnum(line[pos - 1])) &&
                               (pos + targetWord.length() == line.length() || !isalnum(line[pos + targetWord.length()]));
            if (isWholeWord) {
                line.replace(pos, targetWord.length(), newWord);
            }
            pos = line.find(targetWord, pos + newWord.length());
        }
    }


    void appendLine(const string& text) {
        LineNode* newNode = new LineNode(text);
        if (!buffer) {
            buffer = newNode;
        } else {
            LineNode* current = buffer;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }


    void clearBuffer() {
        while (buffer) {
            LineNode* temp = buffer;
            buffer = buffer->next;
            delete temp;
        }
    }


    void saveState() {
        undoStack.push(cloneBuffer());
        cout<<"Word inserted successfully!!"<<endl;
    }


    LineNode* cloneBuffer() const {
        if (!buffer) return nullptr;
        LineNode* head = new LineNode(buffer->text);
        LineNode* current = head;
        LineNode* original = buffer->next;
        while (original) {
            current->next = new LineNode(original->text);
            current = current->next;
            original = original->next;
        }
        return head;
    }
void displayMenu() const {
        cout << "\nCommands:\n";
        cout << "  insert       - Insert text\n";
        cout << "  display      - Display current text\n";
        cout << "  undo         - Undo last change\n";
        cout << "  addrel       - Add word relationship\n";
        cout << "  connections  - Display word connections\n";
        cout << "  search       - Search for a word\n";
        cout << "  replace      - Replace a word\n";
        cout << "  ignore       - Ignore a word for spellcheck\n";
        cout << "  adddict      - Add word to personal dictionary\n";
        cout << "  exit         - Exit the editor\n";
    }


    void loadDefaultDictionary() {
    // Core dictionary words (expanded categories)
    dictionary = {
        "the", "quick", "brown", "fox", "jumps", "over", "lazy", "dog",
        "apple", "banana", "cherry", "date", "elderberry", "fig", "grape",
        "house", "island", "jungle", "kite", "lemon", "mountain", "notebook",
        "ocean", "parrot", "queen", "river", "sun", "tree", "umbrella",
        "village", "water", "xylophone", "yellow", "zebra", "computer",
        "laptop", "keyboard", "mouse", "screen", "speaker", "microphone",
        "camera", "software", "hardware", "network", "internet", "router",
        "algorithm", "function", "variable", "loop", "condition", "program",
        "engineer", "developer", "scientist", "doctor", "artist", "writer",
        "teacher", "student", "school", "university", "hospital", "library",
        "road", "car", "bicycle", "train", "airplane", "ship", "satellite",
        "earth", "moon", "star", "galaxy", "planet", "space", "astronaut",
        "energy", "power", "electricity", "battery", "current", "voltage",
        "circuit", "resistor", "capacitor", "inductor", "transistor",
        "processor", "memory", "storage", "file", "folder", "database",
        "server", "client", "protocol", "encryption", "password", "security",
        "health", "fitness", "exercise", "nutrition", "protein", "vitamin",
        "mineral", "water", "hydration", "sleep", "mental", "wellness",
        "happiness", "motivation", "goal", "success", "achievement",
        "growth", "progress", "development", "knowledge", "wisdom",
        "curiosity", "imagination", "creativity", "innovation", "discovery",
        "exploration", "adventure", "journey", "destination", "purpose",
        "freedom", "justice", "equality", "community", "friendship",
        "family", "love", "compassion", "kindness", "patience", "peace",
        "respect", "gratitude", "forgiveness", "courage", "strength",
        "determination", "resilience", "faith", "hope", "trust", "truth",
        "honesty", "integrity", "responsibility", "loyalty", "humility",
        "generosity", "empathy", "dignity", "pride", "humor", "harmony",
        "balance", "simplicity", "beauty", "elegance", "quality", "excellence",
        "precision", "accuracy", "clarity", "focus", "vision", "ambition",
        "strategy", "planning", "execution", "analysis", "research",
        "evaluation", "measurement", "optimization", "performance",
        "efficiency", "effectiveness", "productivity", "innovation",
        "creativity", "teamwork", "collaboration", "communication",
        "leadership", "management", "organization", "coordination",
        "decision", "solution", "opportunity", "challenge", "problem",
        "risk", "reward", "benefit", "value", "impact", "outcome", "result",
        "success", "failure", "lesson", "experience", "knowledge",
        "understanding", "awareness", "insight", "perspective", "learning",
        "growth", "development", "progress", "advancement", "achievement",
        "milestone", "goal", "vision", "mission", "purpose", "direction",
        "focus", "clarity", "simplicity", "elegance", "quality", "excellence",
        "precision", "accuracy", "consistency", "reliability", "durability",
        "stability", "flexibility", "adaptability", "scalability",
        "efficiency", "effectiveness", "productivity", "performance",
        "capability", "competence", "expertise", "skill", "knowledge",
        "intelligence", "wisdom", "creativity", "innovation", "imagination",
        "curiosity", "exploration", "discovery", "invention", "design",
        "engineering", "development", "implementation", "execution",
        "operation", "maintenance", "support", "service", "delivery",
        "customer", "client", "user", "stakeholder", "community", "society"
    };


    // Add placeholders to reach a total of at least 500 words
    for (int i = 1; i <= 500; ++i) {
        dictionary.insert("example" + std::to_string(i));
    }
}


// Function to load common misspellings
void loadCommonMisspellings() {
    // Load a mapping of common misspellings to their correct forms
    commonMisspellings = {
        {"teh", "the"}, {"quik", "quick"}, {"brwn", "brown"}, {"fok", "fox"},
        {"jmps", "jumps"}, {"ovr", "over"}, {"lzy", "lazy"}, {"dg", "dog"},
        {"aplpe", "apple"}, {"bananna", "banana"}, {"cheery", "cherry"},
        {"dat", "date"}, {"elderbery", "elderberry"}, {"figg", "fig"}, {"grap", "grape"},
        {"huse", "house"}, {"islend", "island"}, {"jungel", "jungle"},
        {"kiet", "kite"}, {"lemmon", "lemon"}, {"mountin", "mountain"},
        {"noteook", "notebook"}, {"ocan", "ocean"}, {"parot", "parrot"},
        {"quene", "queen"}, {"rivver", "river"}, {"sunn", "sun"},
        {"trea", "tree"}, {"umbrelle", "umbrella"}, {"villige", "village"},
        {"watter", "water"}, {"xylophonne", "xylophone"}, {"yelo", "yellow"},
        {"zebbra", "zebra"}, {"computor", "computer"}, {"lapptop", "laptop"},
        {"keybord", "keyboard"}, {"mose", "mouse"}, {"scereen", "screen"},
        {"speeker", "speaker"}, {"microphonne", "microphone"}, {"cammera", "camera"},
        {"softwere", "software"}, {"hardwere", "hardware"}, {"netwrok", "network"},
        {"internt", "internet"}, {"routr", "router"}, {"algoritm", "algorithm"},
        {"funtion", "function"}, {"varible", "variable"}, {"looop", "loop"},
        {"conditon", "condition"}, {"progam", "program"}, {"enginer", "engineer"},
        {"develper", "developer"}, {"scientiest", "scientist"}, {"docter", "doctor"},
        {"artst", "artist"}, {"writor", "writer"}, {"techer", "teacher"},
        {"studnt", "student"}, {"schhol", "school"}, {"unversity", "university"},
        {"hosptal", "hospital"}, {"libary", "library"}, {"rode", "road"},
        {"carr", "car"}, {"bicycal", "bicycle"}, {"trane", "train"},
        {"airplne", "airplane"}, {"shipt", "ship"}, {"satlitte", "satellite"},
        {"erth", "earth"}, {"mon", "moon"}, {"str", "star"}, {"galxy", "galaxy"},
        {"plannet", "planet"}, {"spce", "space"}, {"astronot", "astronaut"},
        {"enerjy", "energy"}, {"powr", "power"}, {"electricty", "electricity"},
        {"battary", "battery"}, {"currnt", "current"}, {"voltge", "voltage"},
        {"circut", "circuit"}, {"resistorr", "resistor"}, {"capaciter", "capacitor"},
        {"indctor", "inductor"}, {"transister", "transistor"}, {"procesor", "processor"},
        {"memmory", "memory"}, {"storag", "storage"}, {"fyle", "file"},
        {"foldor", "folder"}, {"datbase", "database"}, {"sevrer", "server"},
        {"cliant", "client"}, {"protocal", "protocol"}, {"encriptian", "encryption"},
        {"passward", "password"}, {"securty", "security"}, {"helth", "health"},
        {"fitnes", "fitness"}, {"excercise", "exercise"}, {"nutriton", "nutrition"},
        {"protin", "protein"}, {"vitamn", "vitamin"}, {"minerel", "mineral"},
        {"waterr", "water"}, {"hydrationn", "hydration"}, {"slep", "sleep"},
        {"mentel", "mental"}, {"wellnes", "wellness"}, {"hapiness", "happiness"},
        {"motvtion", "motivation"}, {"gaol", "goal"}, {"succcess", "success"},
        {"achievment", "achievement"}, {"growh", "growth"}, {"prgress", "progress"},
        {"developmnt", "development"}, {"knowlege", "knowledge"}, {"wisdome", "wisdom"},
        {"curiostiy", "curiosity"}, {"imaginnation", "imagination"}, {"creativty", "creativity"},
        {"innovaton", "innovation"}, {"discovry", "discovery"}, {"explortion", "exploration"},
        {"advnture", "adventure"}, {"journy", "journey"}, {"destinaton", "destination"},
        {"purpse", "purpose"}, {"freedm", "freedom"}, {"justce", "justice"},
        {"equaity", "equality"}, {"commnity", "community"}, {"freindship", "friendship"},
        {"famly", "family"}, {"lve", "love"}, {"compasion", "compassion"},
        {"kindeness", "kindness"}, {"patince", "patience"}, {"pice", "peace"},
        {"respet", "respect"}, {"gratitud", "gratitude"}, {"forgivnes", "forgiveness"},
        {"courge", "courage"}, {"strenght", "strength"}, {"determnation", "determination"},
        {"resilence", "resilience"}, {"fauth", "faith"}, {"hop", "hope"},
        {"trst", "trust"}, {"thruth", "truth"}, {"honsty", "honesty"},
        {"integrty", "integrity"}, {"responsbilty", "responsibility"}, {"loyality", "loyalty"},
        {"humilty", "humility"}, {"generosityy", "generosity"}, {"empthy", "empathy"},
        {"dignityy", "dignity"}, {"pridee", "pride"}, {"humorrr", "humor"},
        {"harmny", "harmony"}, {"balnce", "balance"}, {"simplicityy", "simplicity"},
        {"beuty", "beauty"}, {"elegancee", "elegance"}, {"qualty", "quality"},
        {"excellnce", "excellence"}, {"precison", "precision"}, {"accurracy", "accuracy"},
        {"claritty", "clarity"}, {"focusss", "focus"}, {"vison", "vision"},
        {"ambtion", "ambition"}, {"strtegy", "strategy"}, {"planing", "planning"},
        {"executin", "execution"}, {"analysiss", "analysis"}, {"reasearch", "research"},
        {"evalution", "evaluation"}, {"measurment", "measurement"}, {"optimzation", "optimization"},
        {"performnce", "performance"}, {"efficincy", "efficiency"}, {"effectivness", "effectiveness"},
        {"productivty", "productivity"}, {"innvaton", "innovation"}, {"creativty", "creativity"},
        {"teamwrk", "teamwork"}, {"collabration", "collaboration"}, {"communicatn", "communication"},
        {"leadershipp", "leadership"}, {"managment", "management"}, {"organiztion", "organization"},
        {"coordnation", "coordination"}, {"decison", "decision"}, {"soluton", "solution"},
        {"opportnity", "opportunity"}, {"challnge", "challenge"}, {"problm", "problem"},
        {"rik", "risk"}, {"rewrd", "reward"}, {"benfit", "benefit"},
        {"valeu", "value"}, {"impct", "impact"}, {"outcom", "outcome"},
        {"reslt", "result"}, {"sucess", "success"}, {"failre", "failure"},
        {"lessn", "lesson"}, {"experince", "experience"}, {"knowlege", "knowledge"},
        {"understnding", "understanding"}, {"awarness", "awareness"}, {"insigt", "insight"},
        {"perspectve", "perspective"}, {"learng", "learning"}, {"grwoth", "growth"},
        {"developmnt", "development"}, {"prgress", "progress"}, {"advancemnt", "advancement"},
        {"achievment", "achievement"}, {"mileston", "milestone"}, {"goaal", "goal"},
        {"visin", "vision"}, {"missin", "mission"}, {"purpse", "purpose"},
        {"directon", "direction"}, {"clarityy", "clarity"}, {"simplicty", "simplicity"},
        {"elegnce", "elegance"}, {"qulity", "quality"}, {"excellece", "excellence"},
        {"preciion", "precision"}, {"accurcy", "accuracy"}, {"consistncy", "consistency"},
        {"reliablity", "reliability"}, {"durabilty", "durability"}, {"stablity", "stability"},
        {"flexiblity", "flexibility"}, {"adaptabilty", "adaptability"}, {"scalabilty", "scalability"},
        {"efficiecy", "efficiency"}, {"effectivenss", "effectiveness"}, {"productvity", "productivity"},
        {"performane", "performance"}, {"capabilty", "capability"}, {"competnce", "competence"},
        {"expertie", "expertise"}, {"skil", "skill"}, {"knowldge", "knowledge"},
        {"intellgence", "intelligence"}, {"wisdome", "wisdom"}, {"creatvity", "creativity"},
        {"innvation", "innovation"}, {"imagnation", "imagination"}, {"curiosiy", "curiosity"},
        {"explorationn", "exploration"}, {"discover", "discovery"}, {"invetion", "invention"},
        {"designn", "design"}, {"engneering", "engineering"}, {"developmnt", "development"},
        {"implemntation", "implementation"}, {"execuion", "execution"},
        {"operaton", "operation"}, {"maintnance", "maintenance"}, {"suport", "support"},
        {"servce", "service"}, {"delvery", "delivery"}, {"customr", "customer"},
        {"clent", "client"}, {"userr", "user"}, {"stakehlder", "stakeholder"},
        {"commnity", "community"}, {"socety", "society"}
    };
}
 std::string cleanInput(const std::string& word) const {
        std::string cleanWord;
        for (char c : word) {
            if (std::isalpha(c)) {
                cleanWord += std::tolower(c);  // Convert to lowercase
            }
        }
        return cleanWord;
    }


 void checkSpellingInText(const std::string& text) {
        std::string word;
        for (char c : text) {
            if (std::isalpha(c)) {
                word += std::tolower(c);
            } else if (!word.empty()) {
                word = cleanInput(word);  // Normalize the word
                checkSpelling(word);  // Check for spelling mistakes
                word.clear();
            }
        }
        if (!word.empty()) {
            word = cleanInput(word);  // Check the last word
            checkSpelling(word);
        }
    }
     void checkSpelling(const std::string& word) {
        if (word.empty()) return;
        if (dictionary.find(word) == dictionary.end()) {
            std::cout << "Misspelled word: " << word << "\n";
            suggestCorrections(word);  // Suggest corrections for the misspelled word
        }
    }


    // Suggest corrections for a misspelled word
    void suggestCorrections(const std::string& word) const {
        if (commonMisspellings.find(word) != commonMisspellings.end()) {
            std::cout << "Did you mean: " << commonMisspellings.at(word) << "?\n";
            return;
        }
        std::cout << "Suggestions: ";
        for (const auto& dictWord : dictionary) {
            if (dictWord.find(word) != std::string::npos) {  // Simple substring matching
                std::cout << dictWord << " ";
            }
        }
        std::cout << "\n";
    }


    // Count the number of words in the buffer
    size_t countWords() const {
    size_t count = 0;
    LineNode* current = buffer;


    while (current) { // Traverse the linked list
        std::string word;
        for (char c : current->text) { // Process the current line's text
            if (std::isalpha(c)) {
                word += std::tolower(c);
            } else if (!word.empty()) {
                count++;
                word.clear();
            }
        }
        if (!word.empty()) { // Count the last word in the line
            count++;
        }
        current = current->next; // Move to the next line
    }


    return count;
}
};


bool isUserExists(const string& username) {
    ifstream file("users.txt");
    string storedUsername, storedPassword;
    while (file >> storedUsername >> storedPassword) {
        if (storedUsername == username) {
            return true;
        }
    }
    return false;
}


// Function to validate the user's credentials
bool validateCredentials(const string& username, const string& password) {
    ifstream file("users.txt");
    string storedUsername, storedPassword;
    while (file >> storedUsername >> storedPassword) {
        if (storedUsername == username && storedPassword == password) {
            return true;
        }
    }
    return false;
}


// Function to register a new user
void registerUser() {
    cout << "Enter a username: ";
    string username;
    cin >> username;


    // Check if the user already exists
    if (isUserExists(username)) {
        cout << "User already exists. Please log in.\n";
        return;
    }


    cout << "Enter a password: ";
    string password;
    cin >> password;


    // Save the new user to the file
    ofstream file("users.txt", ios::app);
    file << username << " " << password << "\n";
    cout << "Registration successful!\n";
}
int main() {
    // ASCII Art
     setColor(11);
  cout << "                                                                                                 \n";
    cout <<"\t\t\t\t\t\t\t" ",--------.,--.   ,--. ,------. ,--.,--.  ,--. ,----.            ,--.  ,---. ,--------.,--------. \n";
    cout <<"\t\t\t\t\t\t\t" "'--.  .-- '.  `.'  /  |  .--. '|  ||  ,'.|  |'  .-./            |  | /  O  ''--.  .--''--.  .--' \n";
    cout <<"\t\t\t\t\t\t\t" "   |  |     '.    /   |  '--' ||  ||  |' '  ||  | .---.    ,--. |  ||  .-.  |  |  |      |  |    \n";
    cout <<"\t\t\t\t\t\t\t" "   |  |       |  |    |  | --' |  ||  | `   |'  '--'  |    |  '-'  /|  | |  |  |  |      |  |    \n";
    cout <<"\t\t\t\t\t\t\t" "   `--'       `--'    `--'      --' --'  `--' `------'      `-----' `--' `--'  `--'      `--'    \n";
    cout <<"\t\t\t\t\t\t\t" "                                                                                                \n";




    setColor(14); // Reset to default console color
    cout << "\n\t\t\t\t\t\t\t\t\t\t\t Welcome to the Enhanced Text Editor!\n";
        string choice;
    cout << "Do you have an account? (yes/no): ";
    cin >> choice;
    if (choice == "no") {
        registerUser();
    }


    cout << "Enter your username: ";
    string username;
    cin >> username;
    cout << "Enter your password: ";
    string password;
    cin >> password;


    if (validateCredentials(username, password)) {
        cout << "Login successful!\n";
        TextEditor editor;
        editor.run();
    } else {
        cout << "Invalid credentials. Exiting...\n";
    }


    return 0;
}

