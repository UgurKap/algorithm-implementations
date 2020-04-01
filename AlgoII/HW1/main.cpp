#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <chrono>

int max_level;

struct Pokemon{
    std::string name;
    int HP, PP;
};

struct Stats{
    double prob;
    bool isleaf;
    char turn;
    int level;
};

class Node{
public:
    Node* parent;
    bool discovered;
    Pokemon Pikachu{};
    Pokemon Blastoise{};
    Stats St{};
    std::string attackName;
    bool effective;
    std::vector<Node*> nextLevel;

    Node(Pokemon P, Pokemon B, Stats S, std::string attack = std::string("Start"), Node* par = nullptr, bool ef = true){
        discovered = false;
        effective = ef;
        attackName = attack;
        if (B.HP < 0) B.HP = 0;
        if (P.HP < 0) P.HP = 0;
        Pikachu = P;
        Blastoise = B;
        St = S;
        if (St.level == max_level || Blastoise.HP == 0 || Pikachu.HP == 0){
            St.isleaf = true;
        }
        this->parent = par;
    }

    void printAttack();

    std::vector<Node*> generateNodes();
};

std::vector<Node *> Node::generateNodes() {
    if (this->St.isleaf)
        return std::vector<Node *>();

    Stats nextSt{};
    nextSt.level = St.level + 1;
    nextSt.isleaf = nextSt.level == max_level;
    double prob = St.prob;
    Pokemon currentPokemon;

    if(St.turn == 'p'){
        currentPokemon = Pikachu;
    } else {
        currentPokemon = Blastoise;
    }

    if (nextSt.level < 3){
        prob /= 3;
    } else {
        int divideby = 1;
        if(currentPokemon.name == Pikachu.name){
            if (Pikachu.PP >= 20) {divideby = 4;}
            else if (Pikachu.PP >= 15) {divideby = 3;}
            else if (Pikachu.PP >= 10) {divideby = 2;}
        } else {
            if (Blastoise.PP >= 25) {divideby = 4;}
            else if (Blastoise.PP >= 20) {divideby = 3;}
            else if (Blastoise.PP >= 10) {divideby = 2;}
        }
        prob /= divideby;
    }

    nextSt.prob = prob;

    Pokemon nextP = Pikachu;
    Pokemon nextB = Blastoise;

    if (St.turn == 'p'){
        nextSt.turn = 'b';
        // Attack 1 - Thundershock

        if (Pikachu.PP >= 10) {
            nextP.PP -= 10;
            nextB.HP -= 40;

            if (nextB.HP <= 0){
                nextSt.isleaf = true;
            }

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Thundershock"), this));

            nextSt.isleaf = nextSt.level == max_level;

            nextB.HP += 40;
            nextP.PP += 10;
        }
        // Attack 2 - Successful - Skull Bash
        if (Pikachu.PP >= 15) {
            nextP.PP -= 15;
            nextB.HP -= 50;
            nextSt.prob *= .7;

            if (nextB.HP <= 0) {
                nextSt.isleaf = true;
            }

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Skull Bash"), this));

            nextSt.isleaf = nextSt.level == max_level;

            nextSt.prob = prob;
            nextB.HP += 50;
            nextP.PP += 15;

            // Attack 2 - Unsuccessful - Skull Bash

            nextP.PP -= 15;
            nextSt.prob *= .3;

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Skull Bash"), this, false));

            nextSt.prob = prob;
            nextP.PP += 15;
        }
        // Attack 3 - Successful - Slam
        if (Pikachu.PP >= 20) {

            nextP.PP -= 20;
            nextB.HP -= 60;
            nextSt.prob *= .8;

            if (nextB.HP <= 0) {
                nextSt.isleaf = true;
            }

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Slam"), this));


            nextSt.isleaf = nextSt.level == max_level;
            nextSt.prob = prob;
            nextB.HP += 60;
            nextP.PP += 20;

            // Attack 3 - Unsuccessful - Slam

            nextP.PP -= 20;
            nextSt.prob *= .2;

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Slam"), this, false));

            nextSt.prob = prob;
            nextP.PP += 20;
        }
        // Attack 4 - Skip
        if (nextSt.level < 3){
        } else {
            nextP.PP += 100;
            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Skip"), this));
        }
    } else {
        nextSt.turn = 'p';
        // Attack 1 - Tackle
        if (Blastoise.PP >= 10){
            nextB.PP -= 10;
            nextP.HP -= 30;

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Tackle"), this));

            nextP.HP += 30;
            nextB.PP += 10;
        }
        // Attack 2 - Water Gun
        if (Blastoise.PP >= 20) {
            nextB.PP -= 20;
            nextP.HP -= 40;

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Water Gun"), this));

            nextP.HP += 40;
            nextB.PP += 20;
        }
        // Attack 3 - Bite
        if (Blastoise.PP >= 25) {
            nextB.PP -= 25;
            nextP.HP -= 60;

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Bite"), this));

            nextP.HP += 60;
            nextB.PP += 25;
        }
        // Attack 4 - Skip

        if (nextSt.level < 3){
        } else {
            nextB.PP += 100;

            nextLevel.push_back(new Node(nextP, nextB, nextSt, std::string("Skip"), this));
        }

    }

    return nextLevel;
}

void Node::printAttack() {
    std::string name = St.turn == 'b' ? Pikachu.name : Blastoise.name;
    std::cout << name << " used " << attackName << ". It's " << (effective ? "effective.":"not effective.") << std::endl;
}

class Graph{
public:
    int level = 0;
    Node *root;
    Node *easy_final;
    std::unordered_map<Node*, std::vector<Node*>> adj;
    std::vector<Node*> currentLevel;
    std::vector<Node*> nextLevel;

    std::queue<Node*> discovered_bfs;
    std::stack<Node*> discovered_dfs;
    std::vector<Node*> processed;

    Graph();
    void generate();
    void bfs();
    void dfs();
    void find_easiest(char pokemon);
    void print_current_level();
    bool easy_check(char poke);
};

Graph::Graph() {
    Pokemon Pika{}, Bla{};
    Pika.name = "Pikachu";
    Bla.name = "Blastoise";
    Pika.HP = 200;
    Bla.HP = 200;
    Pika.PP = 100;
    Bla.PP = 100;

    Stats st{};
    st.turn = 'p';
    st.prob = 1;
    st.level = 0;
    st.isleaf = false;

    root = new Node(Pika, Bla, st);
    discovered_bfs.push(root);
    discovered_dfs.push(root);
    root->discovered = true;
    currentLevel.push_back(root);
    nextLevel = root->generateNodes();
    adj.insert(std::make_pair(root, nextLevel));

    if (!nextLevel.empty()){
        level = 1;
        currentLevel.swap(nextLevel);
        nextLevel.clear();
    }
}

void Graph::generate() {

    for(auto & it : currentLevel){
        if (it == nullptr) continue;
        std::vector<Node*> generated = it->generateNodes();
        adj.insert(std::make_pair(it, generated));
        adj[it].push_back(it->parent);
        nextLevel.insert(nextLevel.end(), generated.begin(), generated.end());
    }

    if (!nextLevel.empty()){
        level += 1;
        currentLevel.swap(nextLevel);
        nextLevel.clear();
    }

    if(level < max_level){
        generate();
    } else {
        for (auto & it : currentLevel){
            if (it == nullptr) continue;
            adj.insert(std::make_pair(it, std::vector<Node*>(1, it->parent)));
        }
    }
}

void Graph::bfs() {
    auto begin = std::chrono::steady_clock::now();

    while (!(discovered_bfs.empty())){
        for(auto & it: adj[discovered_bfs.front()]){
            if (it->discovered)
                continue;
            discovered_bfs.push(it);
            it->discovered = true;
        }

        processed.push_back(discovered_bfs.front());
        discovered_bfs.pop();
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

    std::cout << "Node count: " << processed.size() << std::endl;
    std::cout << "Time elapsed: " << duration << " ns" << std::endl;
}

void Graph::dfs() {
    auto begin = std::chrono::steady_clock::now();

    while (!(discovered_dfs.empty())){

        Node * cur = discovered_dfs.top();
        discovered_dfs.pop();

        for(auto & it: adj[cur]){
            if (it->discovered)
                continue;
            discovered_dfs.push(it);
            it->discovered = true;
        }

        processed.push_back(cur);

    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

    std::cout << "Node count: " << processed.size() << std::endl;
    std::cout << "Time elapsed: " << duration << " ns" << std::endl;
}

bool Graph::easy_check(char poke) {
    for (auto it = currentLevel.begin(); it != currentLevel.end(); ++it){
        if(*it == nullptr){
            continue;
        }

        if((*it)->St.isleaf && (((*it)->Pikachu.HP == 0 && poke == 'b')||((*it)->Blastoise.HP == 0 && poke == 'p'))){
            easy_final = (*it);
            return true;
        }
    }

    return false;
}

void Graph::find_easiest(char pokemon) {
    if(!(pokemon == 'p' || pokemon == 'b')){
        std::cerr << "You have supplied an unknown commandline argument." << std::endl;
        exit(1);
    }

    // We will be using a variant of breadth-first search.

    // std::vector<Node*> currentLevel;
    // std::vector<Node*> nextLevel;
    max_level += 1;
    while(!this->easy_check(pokemon)){
        for (auto it = currentLevel.begin(); it != currentLevel.end(); ++it){
            if(*it == nullptr){
                continue;
            }
            std::vector<Node*> gen = (*it)->generateNodes();
            nextLevel.insert(nextLevel.end(), gen.begin(), gen.end());
        }
        currentLevel.swap(nextLevel);
        nextLevel.clear();
        max_level += 1;
    }

    int level_c = easy_final->St.level;
    double pr = easy_final->St.prob;

    std::vector<Node*> match;
    match.push_back(easy_final);

    while (easy_final->parent != nullptr){
        match.push_back(easy_final->parent);
        easy_final = easy_final->parent;
    }

    for(auto it = match.rbegin(); it != match.rend(); ++it){
        if ((*it)->parent != nullptr) (*it)->printAttack();
    }

    std::cout << "Level count: " << level_c << std::endl;
    std::cout << "Probability: " << pr << std::endl;
}

void Graph::print_current_level() {
    for (auto & it : currentLevel){
        if (it == nullptr){
            continue;
        }

        std::cout << "P_HP:" << it->Pikachu.HP << " P_PP:" << it->Pikachu.PP << " B_HP:" << it->Blastoise.HP;
        std::cout << " B_PP:" << it->Blastoise.PP << " PROB:" << it->St.prob << std::endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 3){
        std::cerr << "You have to supply more information." << std::endl;
        exit(1);
    } else if (argc > 4){
        std::cerr << "You have to supply less information." << std::endl;
        exit(1);
    }

    Graph *g;

    std::string part = argv[1];

    if (part == std::string("part1")){
        try {
            max_level = std::stoi(argv[2]);
        } catch(std::invalid_argument& err){
            std::cerr << "Please supply the max level in integer format as your second argument." << std::endl;
            exit(1);
        }

        g = new Graph();
        g->generate();
        g->print_current_level();

    } else if (part == std::string("part2")) {
        if (argc != 4){
            std::cerr << "You have to supply more information." << std::endl;
            exit(1);
        }

        try {
            max_level = std::stoi(argv[2]);
        } catch(std::invalid_argument& err){
            std::cerr << "Please supply the max level in integer format as your second argument." << std::endl;
            exit(1);
        }

        g = new Graph();
        g->generate();

        if (std::string(argv[3]) == std::string("dfs")){
            g->dfs();
        } else if (std::string(argv[3]) == std::string("bfs")){
            g->bfs();
        } else {
            std::cerr << "You have supplied an unknown commandline argument." << std::endl;
            exit(1);
        }

    } else if (part == std::string("part3")){
        max_level = 5;
        g = new Graph();
        argv[2][0] = (char)tolower(argv[2][0]);

        if(!(std::string(argv[2]) == std::string("pikachu") || std::string(argv[2]) == std::string("blastoise"))){
            std::cerr << "Unknown Pokemon" << std::endl;
            exit(1);
        }
        g->find_easiest(argv[2][0]);

    } else {
        std::cerr << "You have supplied an unknown commandline argument." << std::endl;
        exit(1);
    }

    return 0;
}