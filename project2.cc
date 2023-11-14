/*
 * Modified by Aditya Ranjit Kotwal, 2023
 */
 
/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include "lexer.h"
#include <algorithm>
#include <utility>
#include <map>
using namespace std;
LexicalAnalyzer lexer;

struct CharacterType
// Structure to store terminals and non terminals
{
    std::vector<std::string> non_terminals;
    std::vector<std::string> terminals;
};

struct Rule
// Structure to define a Rule LHS -> RHS
{
    std::string lhs;
    std::vector<std::string> rhs;
};

typedef std::unordered_map<std::string, std::vector<std::string>> Fsets;

Fsets FirstSet;
Fsets FollowSet;

void syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}

bool doesRuleExist(std::vector<Rule> &rules, std::string non_terminal)
// Function to check if a rule of a particular non terminal exists
{
    for (auto each_rule : rules)
    {
        if (each_rule.lhs == non_terminal)
            return true;
    }
    return false;
}

void addRule(std::vector<Rule> &rules, std::string lhs, vector<std::string> rhs)
// Function to check if a rule of a particular non terminal exists
{
    Rule r;
    if (!doesRuleExist(rules, lhs))
    {
        r.lhs = lhs;
        r.rhs = rhs;
        rules.push_back(r);
        return;
    }
    r.lhs = lhs;

    for (auto i : rhs)
    {
        r.rhs.push_back(i);
    }
    rules.push_back(r);
    return;
}

Token expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

void readIdList(vector<std::string> &rhs_rule)
{
    Token t = lexer.peek(1);
    rhs_rule.push_back(t.lexeme);
    expect(ID);
    t = lexer.peek(1);
    if (t.token_type == STAR)
    {
        return;
    }
    else if (t.token_type == ID)
    {
        readIdList(rhs_rule);
    }
    else
        syntax_error();
}
void readRHS(vector<std::string> &rhs_rule)
{
    Token t = lexer.peek(1);
    if (t.token_type == STAR)
    {
        if (!rhs_rule.size())
            rhs_rule.push_back("#");

        return;
    }
    else if (t.token_type == ID)
    {
        readIdList(rhs_rule);
    }

    else
        syntax_error();
}

// A -> A b B C
//      ^
void readRule(std::vector<Rule> &rules)
{

    Token t = lexer.peek(1);
    std::string current_non_terminal;
    current_non_terminal = t.lexeme; // A
    expect(ID);

    expect(ARROW);

    vector<std::string> rhs_rule;
    readRHS(rhs_rule);

    addRule(rules, current_non_terminal, rhs_rule);

    expect(STAR);
}

void readRuleList(std::vector<Rule> &rules)
{
    Token t = lexer.peek(1);
    if (t.token_type == STAR)
    {
        expect(STAR);
        return;
    }
    else if (t.token_type == ID)
    {
        readRule(rules);
        readRuleList(rules);
    }
    else
        return;
}

// read grammar
void readGrammar(std::vector<Rule> &rules)
{
    readRuleList(rules);
    expect(HASH);
    expect(END_OF_FILE);
}

bool isNonTerminal(vector<std::string> non_terminals, string character)
{
    for (std::string str : non_terminals)
    {
        if (str == character)
        {
            return true;
        }
    }
    return false;
}

void addNonTerminal(vector<std::string> &non_terminals, string non_terminal)
// Function that adds a non-terminal to the list of other non-terminals
{
    for (auto it : non_terminals)
    {
        if (it == non_terminal)
            return;
    }
    non_terminals.push_back(non_terminal);
}

void addTerminal(vector<std::string> &terminals, string terminal)
// Function that adds a non-terminal to the list of other non-terminals
{
    for (auto it : terminals)
    {
        if (it == terminal)
            return;
    }
    terminals.push_back(terminal);
}

CharacterType fetchTypes(std::vector<Rule> rules)
// Function that finds terminals and non-terminals given a set of rules
{
    vector<std::string> non_terminals;
    vector<std::string> final_non_terminals;
    vector<std::string> terminals;
    for (auto it : rules)
    // If any symbol on the RHS exists on the LHS, it is a non-terminal
    {
        bool exists = false;
        for (auto rule : non_terminals)
        {
            if (rule == it.lhs)
            {
                exists = true;
            }
        }
        if (!exists)
            non_terminals.push_back(it.lhs);
    }

    for (auto rule : rules)
    {
        // Add all non terminals of the RHS to the existing list of non terminals
        // in the order of appearance
        if (isNonTerminal(non_terminals, rule.lhs))
            addNonTerminal(final_non_terminals, rule.lhs);
        for (auto each_rhs_rule : rule.rhs)
        {
            // if each symbol on the RHS of current rule is not a non-terminal, add it to terminal
            // and vice versa
            if (!isNonTerminal(non_terminals, each_rhs_rule))
            {
                addTerminal(terminals, each_rhs_rule);
            }
            else
            {
                addNonTerminal(final_non_terminals, each_rhs_rule);
            }
        }
    }
    CharacterType c;
    c.terminals = terminals;
    c.non_terminals = final_non_terminals;
    return c;
}

// Task 1
void Task1(std::vector<Rule> rules)
{
    // Fetch all terminals and non terminals
    CharacterType c = fetchTypes(rules);

    for (std::string t : c.terminals)
    {
        if (t != "#")
            cout << t << " ";
    }

    for (std::string nt : c.non_terminals)
    {
        if (nt != "#")
            cout << nt << " ";
    }
}

bool customCompare(const std::string &left, const std::string &right, const std::vector<std::string> &order)
{
    if (left == "#")
        return false;
    if (right == "#")
        return false;
    if (left == "$")
        return true;
    if (right == "$")
        return false;

    auto leftPos = std::find(order.begin(), order.end(), left);
    auto rightPos = std::find(order.begin(), order.end(), right);
    return std::distance(order.begin(), leftPos) < std::distance(order.begin(), rightPos);
}

void sortStringVectorsInMap(Fsets &mapOfVectors, const std::vector<std::string> &order)
{
    for (auto &entry : mapOfVectors)
    {
        std::vector<std::string> &vec = entry.second;
        std::sort(vec.begin(), vec.end(), [&order](const std::string &left, const std::string &right)
                  { return customCompare(left, right, order); });
    }
}

Fsets findFirstSets(CharacterType c, std::vector<Rule> rules)
{
    for (auto terminal : c.terminals)
    //Initialize First sets of all terminals as themselves
    {
        FirstSet[terminal] = {terminal};
    }

    for (auto non_terminals : c.non_terminals)
    //Initialize First sets of all non-terminals as empty
    {
        FirstSet[non_terminals] = {};
    }
    bool changed = true;
    while (changed)
    //Algorithm keeps proceeding as long as there is any change in any first set
    {
        changed = false;
        for (auto rule : rules)
        {
            std::string lhs = rule.lhs;
            std::vector<std::string> rhs = rule.rhs;
            std::vector<std::string> current_LHS_set = FirstSet[lhs];
            std::vector<std::string> original_set = current_LHS_set;

            bool epsilon_in_all = false;
            bool skip_rule = false;
            for (auto each_rhs : rhs)
            {
                std::vector<std::string> current_first_set = FirstSet[each_rhs];
                if (current_first_set.empty())
                {
                    epsilon_in_all = false;
                    skip_rule = true;
                    break;
                }

                for (auto each_rhs_first : current_first_set)
                {

                    if (each_rhs_first != "#")
                    {
                        auto iterator = std::find(current_LHS_set.begin(), current_LHS_set.end(), each_rhs_first);
                        if (iterator == current_LHS_set.end())
                        {
                            current_LHS_set.push_back(each_rhs_first);
                        }
                    }
                }
                auto it = std::find(current_first_set.begin(), current_first_set.end(), "#");
                if (it == current_first_set.end())
                {
                    epsilon_in_all = false;
                    break;
                }
                else
                {
                    epsilon_in_all = true;
                }
            }
            if (skip_rule && current_LHS_set.empty())
            {
                continue;
            }
            if (epsilon_in_all)
            {
                std::vector<std::string> original_set = current_LHS_set;
                std::vector<std::string> epsilon_set = {"#"};
                for (auto each_element : current_LHS_set)
                {
                    auto it = std::find(epsilon_set.begin(), epsilon_set.end(), each_element);
                    if (it == epsilon_set.end())
                    {
                        epsilon_set.push_back(each_element);
                    }
                }

                current_LHS_set = epsilon_set;
            }
            if (original_set != current_LHS_set)
                changed = true;

            FirstSet[lhs] = current_LHS_set;
        }
    }
    return FirstSet;
}

Fsets findFollowSets(CharacterType c, std::vector<Rule> rules, Fsets FirstSet)
{
    for (auto terminal : c.terminals)
    {
        FollowSet[terminal] = {};
    }

    for (int i = 0; i < c.non_terminals.size(); i++)
    {
        if (i == 0)
        {
            FollowSet[c.non_terminals[i]] = {"$"};
        }
        else
        {
            FollowSet[c.non_terminals[i]] = {};
        }
    }

    for (auto rule : rules)
    {
        std::vector<std::string> rhs = rule.rhs;
        for (int i = 0; i < rhs.size() - 1; i++)
        {
            auto it = std::find(c.terminals.begin(), c.terminals.end(), rhs[i]);
            if (it != c.terminals.end())
            {
                continue;
            }
            std::vector<std::string> current_set = {};

            for (int j = i + 1; j < rhs.size(); j++)
            {
                bool epsilon_found = false;
                for (int k = 0; k < FirstSet[rhs[j]].size(); k++)
                {
                    auto it = std::find(current_set.begin(), current_set.end(), FirstSet[rhs[j]][k]);
                    if (it == current_set.end())
                    {
                        if (FirstSet[rhs[j]][k] == "#")
                        {
                            epsilon_found = true;
                            continue;
                        }
                        current_set.push_back(FirstSet[rhs[j]][k]);
                    }
                }
                if (epsilon_found)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            for (auto x : current_set)
            {
                auto it = std::find(FollowSet[rhs[i]].begin(), FollowSet[rhs[i]].end(), x);
                if (it == FollowSet[rhs[i]].end())
                    FollowSet[rhs[i]].push_back(x);
            }
        }
    }
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (auto rule : rules)
        {
            std::string lhs = rule.lhs;
            std::vector<std::string> rhs = rule.rhs;
            int size_of_rhs = rhs.size();
            for (int i = size_of_rhs - 1; i > -1; i--)
            {
                auto iterator = std::find(c.terminals.begin(), c.terminals.end(), rhs[i]);

                // If met with a terminal stop this rule
                if (FollowSet[lhs].empty() || iterator != c.terminals.end())
                {
                    break;
                }

                std::vector<std::string> original_rhs = FollowSet[rhs[i]];

                // Copying values of LHS to RHS
                for (auto each_set_item : FollowSet[lhs])
                {
                    auto it = std::find(FollowSet[rhs[i]].begin(), FollowSet[rhs[i]].end(), each_set_item);
                    if (it == FollowSet[rhs[i]].end())
                        FollowSet[rhs[i]].push_back(each_set_item);
                }
                auto it = std::find(FirstSet[rhs[i]].begin(), FirstSet[rhs[i]].end(), "#");
                if (original_rhs != FollowSet[rhs[i]])
                    changed = true;
                if (it == FirstSet[rhs[i]].end())
                {

                    break;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    return FollowSet;
}

// Task 2
void Task2(CharacterType c, std::vector<Rule> rules)
{
    // Find first sets of all rules
    FirstSet = findFirstSets(c, rules);

    // Sorting to ensure order of appearance and # on the extreme left
    sortStringVectorsInMap(FirstSet, c.terminals);
    for (auto it : c.non_terminals)
    {
        cout << "FIRST(" << it << ") = { ";
        if (FirstSet[it].size() > 0)
        {
            for (int j = 0; j < FirstSet[it].size() - 1; j++)
            {
                cout << FirstSet[it][j] << ", ";
            }
            cout << FirstSet[it][FirstSet[it].size() - 1];
        }
        cout << " }" << endl;
    }
    cout << endl;
}

Fsets formatForTask3(Fsets FollowSets, std::vector<std::string> terminals)
{   

    // Sorting to ensure order of appearance and $ on the extreme left
    sortStringVectorsInMap(FollowSets, terminals);
    for (auto f : FollowSets)
    {

        std::vector<std::string> elements = FollowSets[f.first];
        auto iter = std::find(elements.begin(), elements.end(), "$");
        if (iter != elements.end())
        {
            std::vector<std::string> temp_vec = {"$"};
            for (auto x : elements)
            {
                if (x != "$")
                {
                    temp_vec.push_back(x);
                }
            }
            FollowSets[f.first] = temp_vec;
        }
    }

    return FollowSets;
}

// Task 3
void Task3(CharacterType c, std::vector<Rule> rules)
{
    //Find first sets
    Fsets first_sets = findFirstSets(c, rules);
    //Find follow sets
    FollowSet = findFollowSets(c, rules, first_sets);
    //Format follow sets as required the output
    Fsets FollowSets = formatForTask3(FollowSet, c.terminals);

    for (auto it : c.non_terminals)
    {
        cout << "FOLLOW(" << it << ") = { ";
        if (FollowSets[it].size() > 0)
        {
            for (int j = 0; j < FollowSets[it].size() - 1; j++)
            {
                cout << FollowSets[it][j] << ", ";
            }
            cout << FollowSets[it][FollowSets[it].size() - 1];
        }
        cout << " }" << endl;
    }
}

bool sortRulesComparator(Rule a, Rule b)
{

    if (a.lhs < b.lhs)
    {
        return true;
    }
    else if (a.lhs > b.lhs)
        return false;
    else if (a.lhs == b.lhs)
    {
        return a.rhs < b.rhs;
    }
}

void removeFromRules(std::vector<Rule> &rules, Rule rule)
{
    std::vector<Rule> temp;
    for (auto x : rules)
    {
        if (x.lhs == rule.lhs && x.rhs == rule.rhs)
            continue;
        temp.push_back(x);
    }
    rules = temp;
}

void addToRules(std::vector<Rule> &rules, Rule rule)
{
    bool duplicate = false;
    for (auto r : rules)
    {
        if (r.lhs == rule.lhs && r.rhs == rule.rhs)
        {
            duplicate = true;
            break;
        }
    }
    if (!duplicate)
        rules.push_back(rule);
}

void task4PrintRules(std::vector<Rule> rules)
{

    for (const auto &pair : rules)
    {
        if (pair.rhs.size() < 0)
            continue;
        std::cout << pair.lhs << " -> ";

        for (const std::string &value : pair.rhs)
        {
            if (value == "#")
                continue;
            std::cout << value << " ";
        }
        std::cout << "#";
        std::cout << std::endl;
    }
}

struct Task4LongestMatch
{
    Rule rule;
    int longest_match;
};

std::vector<std::string> task4ExtractPrefixOfSize(Rule rule, int prefix_size)
{
    std::vector<std::string> extracted_prefix;
    if (prefix_size > rule.rhs.size())
        return {};
    for (int i = 0; i < prefix_size; i++)
    {
        extracted_prefix.push_back(rule.rhs[i]);
    }
    return extracted_prefix;
}

bool sortTask4LongestMatch(Task4LongestMatch a, Task4LongestMatch b)
{

    return a.longest_match > b.longest_match;
}

bool sortTask4Lexicographical(Task4LongestMatch a, Task4LongestMatch b)
{

    if (a.longest_match != b.longest_match)
    {
        return a.longest_match > b.longest_match;
    }
    else
    {
        // If longest_match is the same, compare the entire Rule (LHS + RHS)
        std::vector<std::string> rule_1 = a.rule.rhs;
        rule_1.insert(rule_1.begin(), a.rule.lhs);

        std::vector<std::string> rule_2 = b.rule.rhs;
        rule_2.insert(rule_2.begin(), b.rule.lhs);

        return rule_1 < rule_2;
    }
}

void task4SplitRules(std::string non_terminal, std::vector<Rule> rules, std::vector<Rule> &common_group, std::vector<Rule> &uncommon_group, std::vector<std::string> &suffix)
{
    std::vector<Rule> selected_rules;
    std::vector<Task4LongestMatch> all_matches;
    // Select all rules of selected non terminal
    for (int i = 0; i < rules.size(); i++)
    {

        if (rules[i].lhs == non_terminal)
        {
            selected_rules.push_back(rules[i]);
        }
    }

    // find longest prefix
    for (int i = 0; i < selected_rules.size(); i++)
    {
        Rule first_rule = selected_rules[i];
        Task4LongestMatch m;
        m.rule = first_rule;
        int longest_match_for_first_rule = 0;
        for (int j = 0; j < selected_rules.size(); j++)
        {
            // skipping the same rule
            if (i == j)
                continue;

            Rule second_rule = selected_rules[j];

            int minimum_of_two = min(first_rule.rhs.size(), second_rule.rhs.size());
            int longest_match_with_current_rule = 0;

            for (int k = 0; k < minimum_of_two; k++)
            {
                if (first_rule.rhs[k] == second_rule.rhs[k])
                {
                    ++longest_match_with_current_rule;
                }
                else
                    break;
            }

            longest_match_for_first_rule = max(longest_match_for_first_rule, longest_match_with_current_rule);
        }
        m.longest_match = longest_match_for_first_rule;
        all_matches.push_back(m);
    }
    /*
    1. A -> A B C D longest_match = 2 // with rule 3
    2. B -> A B C D longest_match = 1 // with rules 5 and 6
    3. A -> A B D E longest_match = 2 // with rule 1
    */
    //Sort by longest match
    std::sort(all_matches.begin(), all_matches.end(), sortTask4LongestMatch);
    //Sort lexicographically
    std::sort(all_matches.begin(), all_matches.end(), sortTask4Lexicographical);
    /*
    1. A -> A B C D longest_match = 2 // with rule 3
    3. A -> A B D E longest_match = 2 // with rule 1
    2. B -> A B C D longest_match = 1 // with rules 5 and 6
    */

    // if no rule matches
    int longest_match = all_matches[0].longest_match;
    if (longest_match == 0)
    {
        for (int i = 0; i < all_matches.size(); i++)
            uncommon_group.push_back(all_matches[i].rule);

        common_group = {};
        return;
    }

    // If atleast 1 match is present, split the rules into 2 groups
    std::vector<std::string> longest_prefix = task4ExtractPrefixOfSize(all_matches[0].rule, longest_match);
    suffix = longest_prefix;
    common_group.push_back(all_matches[0].rule);
    for (int i = 1; i < all_matches.size(); i++)
    {
        std::vector<std::string> current_prefix = task4ExtractPrefixOfSize(all_matches[i].rule, longest_match);
        if (longest_prefix == current_prefix)
            //All rules that begin with ⍺
            common_group.push_back(all_matches[i].rule);
        else
            //All rules that do not begin with ⍺
            uncommon_group.push_back(all_matches[i].rule);
    }
}

// Task 4
void Task4(CharacterType c, std::vector<Rule> rules)
{
    std::vector<std::string> new_non_terminals;
    std::vector<Rule> new_rules;
    std::vector<std::string> non_terminals = c.non_terminals;
    unordered_map<std::string, int> counter_values;

    for (auto nt : non_terminals)
        counter_values[nt] = 1;

    while (non_terminals.size())
    {
        int i = 0;
        while (i < non_terminals.size())
        {
            std::vector<Rule> common, uncommon;
            std::vector<std::string> suffix;
            std::string selected_non_terminal = non_terminals[i];
            task4SplitRules(selected_non_terminal, rules, common, uncommon, suffix);
            if (common.size() >= 2)
            {
                // remove common rules
                for (int k = 0; k < common.size(); k++)
                {
                    removeFromRules(rules, common[k]);
                }

                // add the rule A -> ⍺Anew to R
                std::string new_name = selected_non_terminal + to_string(counter_values[selected_non_terminal]++);
                std::vector<std::string> new_rhs = suffix;
                new_rhs.push_back(new_name);
                Rule r;
                r.lhs = selected_non_terminal;
                r.rhs = new_rhs;
                addToRules(rules, r);

                // add the rule Anew -> β to R'
                for (int k = 0; k < common.size(); k++)
                {
                    Rule r;
                    r.lhs = new_name;
                    std::vector<std::string>::const_iterator first = common[k].rhs.begin() + suffix.size();
                    std::vector<std::string>::const_iterator last = common[k].rhs.end();
                    std::vector<std::string> beta(first, last);
                    r.rhs = beta;
                    addToRules(new_rules, r);
                }

                // add Anew to NT'
                new_non_terminals.push_back(new_name);
                i++;
            }
            else
            {
                //If there are no 2 non empty prefix rules
                int j = 0;
                while (j < rules.size())
                {
                    if (rules[j].lhs == selected_non_terminal)
                    {
                        //Add the rule to new rules
                        addToRules(new_rules, rules[j]);
                        //Remove the rule from old rules
                        removeFromRules(rules, rules[j]);
                    }
                    else
                        j++;
                }
                //Remove the non terminal from old non terminals
                auto it = std::find(non_terminals.begin(), non_terminals.end(), selected_non_terminal);
                non_terminals.erase(it);
                //Add the non terminal to new non terminals
                new_non_terminals.push_back(selected_non_terminal);
            }
        }
    }

    //Sort lexicographically
    std::sort(new_rules.begin(), new_rules.end(), sortRulesComparator);
    task4PrintRules(new_rules);
}

struct Task5Rules
//Structure to group all rules of a particular non-terminal together
{
    std::string lhs;
    std::vector<Rule> rhs;
};

std::vector<Rule> sortForTask5(std::vector<Task5Rules> rules)
{
    // Sort the outer vector lexicographically based on 'lhs'
    std::sort(rules.begin(), rules.end(),
              [](Task5Rules &a, Task5Rules &b)
              {
                  return a.lhs < b.lhs;
              });

    std::vector<Rule> inner_rule;
    // Sort the inner Rule vectors lexicographically based on 'lhs'
    for (Task5Rules &task5Rule : rules)
    {
        for (auto x : task5Rule.rhs)
            inner_rule.push_back(x);
    }

    std::sort(inner_rule.begin(), inner_rule.end(), sortRulesComparator);
    return inner_rule;
}

void printTask5Rules(std::vector<Task5Rules> &rules)
{
    std::vector<Rule> inner_rule = sortForTask5(rules);

    for (auto rule : inner_rule)
    {
        std::cout << rule.lhs << " -> ";

        for (std::string &rhs : rule.rhs)
        {
            if (rhs != "#")
                std::cout << rhs << " ";
        }
        cout << "# ";
        cout << endl;
    }
}

// Task 5
void Task5(CharacterType c, std::vector<Rule> rule)
{
    std::vector<Task5Rules> Rules;
    std::vector<std::string> non_terminals;
    non_terminals = c.non_terminals;
    std::vector<Task5Rules> Rules_1;
    for (std::string nt : non_terminals)
    {
        Task5Rules r;
        r.lhs = nt;
        r.rhs = {};
        Rules.push_back(r);
    }
    bool epsilon_found = false;
    for (Task5Rules &task5rule : Rules)
    {
        for (Rule r : rule)
        {
            if (r.rhs[0] == "#")
            {
                epsilon_found = true;
            }
            if (task5rule.lhs == r.lhs)
            {
                task5rule.rhs.push_back(r);
            }
        }
    }
    if (epsilon_found)
    {
        printTask5Rules(Rules);
        exit(1);
    }

    // NT' = NT sorted lexicographically (dictionary order)
    std::vector<std::string> new_non_terminals = non_terminals;
    std::sort(new_non_terminals.begin(), new_non_terminals.end());
    unordered_map<std::string, int> counter_values;
    for (auto nt : new_non_terminals)
        counter_values[nt] = 1;
    int n = new_non_terminals.size();
    for (int i = 0; i < n; i++)
    {
        int index_i;

        for (int j = 0; j < i; j++)
        {

            // Selecting Ai
            for (index_i = 0; index_i < Rules.size(); index_i++)
            {
                if (Rules[index_i].lhs == new_non_terminals[i])
                {
                    break;
                }
            }
            int k = 0;
            // For all r belongs to Ai
            while (k < Rules[index_i].rhs.size())
            {

                int index_j;
                std::vector<std::string> delta;
                // if r has the form Rules[Ai].rhs -> Aj⍺ where Aj < Ai then
                if (Rules[index_i].rhs[k].lhs == new_non_terminals[i] && Rules[index_i].rhs[k].rhs[0] == new_non_terminals[j])
                {
                    // Store the remaining part of the rule except the first Character of RHS
                    for (int kx = 1; kx < Rules[index_i].rhs[k].rhs.size(); kx++)
                        delta.push_back(Rules[index_i].rhs[k].rhs[kx]);
                    // Remove the Rule r from
                    Rules[index_i].rhs.erase(Rules[index_i].rhs.begin() + k);
                    // select Aj
                    for (index_j = 0; index_j < Rules.size(); index_j++)
                    {
                        if (Rules[index_j].lhs == new_non_terminals[j])
                        {
                            break;
                        }
                    }
                    for (int kx = 0; kx < Rules[index_j].rhs.size(); kx++)
                    {
                        std::vector<std::string> new_rule;
                        Rule rul;
                        new_rule = Rules[index_j].rhs[kx].rhs;
                        new_rule.insert(new_rule.end(), delta.begin(), delta.end());

                        rul.lhs = Rules[index_i].lhs;
                        rul.rhs = new_rule; // concatenate
                        Rules[index_i].rhs.push_back(rul);
                    }
                }
                else
                    k++;
            }
        }
        for (index_i = 0; index_i < Rules.size(); index_i++)
        {
            if (Rules[index_i].lhs == new_non_terminals[i])
            {
                break;
            }
        }

        // Remove immediate left Recursion
        // S -> S A b c G H I F G H E F E F D E B C D *
        // S -> S B C G H I F G H E F E F D E B C D *
        // S -> d E F E F D E B C D *
        // S -> c E F D E B C D *
        std::string new_rule_lhs;
        int k = 0;
        std::vector<Rule> left_recur, no_left_recur;
        for (int k = 0; k < Rules[index_i].rhs.size(); k++) // remove
        {

            if (Rules[index_i].rhs[k].lhs == Rules[index_i].rhs[k].rhs[0])
            {

                left_recur.push_back(Rules[index_i].rhs[k]);
                // S -> S A b c G H I F G H E F E F D E B C D *
                // S -> S B C G H I F G H E F E F D E B C D *
            }
            else
            {
                // S -> d E F E F D E B C D *
                // S -> c E F D E B C D *

                no_left_recur.push_back(Rules[index_i].rhs[k]);
            }
        }

        if (left_recur.size())
        {
            Rules[index_i].rhs = {};
            new_rule_lhs = left_recur[k].lhs + to_string(counter_values[left_recur[k].lhs]++); // S1
            new_non_terminals.push_back(new_rule_lhs);

            for (int k = 0; k < left_recur.size(); k++)
            {

                // Add new terminal to Rule
                Rule r;
                counter_values[new_rule_lhs] = 1;
                Task5Rules R;
                R.lhs = new_rule_lhs;                                                                    // outer S1
                r.lhs = new_rule_lhs;                                                                    // inner S1
                std::vector<std::string> suffix(left_recur[k].rhs.begin() + 1, left_recur[k].rhs.end()); // A b c G H I F G H E F E F D E B C D
                suffix.push_back(new_rule_lhs);                                                          // A b c G H I F G H E F E F D E B C D S1
                r.rhs = suffix;
                R.rhs.push_back(r);
                Rules.push_back(R);

                // Remove from vector

                // Extract remaining
                // Append to end of each other vector
            }
            if (no_left_recur.size())
            {
                for (int k = 0; k < no_left_recur.size(); k++)
                {

                    // Add new terminal to Rule
                    Rule r;
                    r.lhs = Rules[index_i].lhs;                   // inner S1
                    no_left_recur[k].rhs.push_back(new_rule_lhs); // S -> d E F E F D E B C D S1 * // S -> c E F D E B C D S1 *
                    r.rhs = no_left_recur[k].rhs;
                    Rules[index_i].rhs.push_back(r);
                }
            }
        }
    }

    Rules_1 = {};
    std::sort(new_non_terminals.begin(), new_non_terminals.end());

    for (int k = 0; k < new_non_terminals.size(); k++)
    {
        std::string selected_NT = new_non_terminals[k];

        for (int m = 0; m < Rules.size(); m++)
        {
            if (Rules[m].lhs == selected_NT)
            {
                Rules_1.push_back(Rules[m]);
            }
        }
    }
    printTask5Rules(Rules_1);
}
int main(int argc, char *argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    std::vector<Rule> rules;
    readGrammar(rules); // Reads the input grammar from standard input
                        // and represent it internally in data structures
                        // ad described in project 2 presentation file

    switch (task)
    {
    case 1:
        Task1(rules);
        break;

    case 2:
    {
        CharacterType c = fetchTypes(rules);
        Task2(c, rules);
        break;
    }

    case 3:
    {
        CharacterType c = fetchTypes(rules);
        Task3(c, rules);
        break;
    }

    case 4:
    {
        CharacterType c = fetchTypes(rules);
        Task4(c, rules);
        break;
    }

    case 5:
    {
        CharacterType c = fetchTypes(rules);
        Task5(c, rules);
        break;
    }

    default:
        cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}
