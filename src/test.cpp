//
// Created by LYL232 on 2019/12/6.
//

#include "ParseTree.hpp"
#include <iostream>

using namespace std;

void test_parse_type() {
    typedef ParseType PType;
    PType type1(T_INT), type2 = PType::get_pointer(type1, 1), type3 = PType::get_pointer(type2, 1);
    cout << type1.get_info() << endl;
    cout << type2.get_info() << endl;
    cout << type3.get_info() << endl;
    vector<pair<string, PType> > fields;
    fields.emplace_back("a", type1);
    fields.emplace_back("b", type2);
    fields.emplace_back("c", type3);
    vector<pair<string, size_t> > self_ptr;
    self_ptr.emplace_back("d", 2);
    self_ptr.emplace_back("e", 2);
    auto t8 = PType::get_struct(fields, self_ptr);
    cout << t8.get_info() << endl;
    cout << t8.get_info() << endl;
    cout << t8.field("a").get_info() << endl;
    cout << t8.field("b").get_info() << endl;
    cout << PType::get_array(t8, 32).get_info() << endl;
    PType::print_all_type();
}

void test_make_node() {
    ParseTree tree("root");
    size_t node = tree.new_node("asdsd"),
            node_1 = tree.make_const_node(string("lalala"));
    tree.set_parent(node_1, node);
    typedef ParseType PType;
    PType type1(T_INT), type2 = PType::get_pointer(type1, 1), type3 = PType::get_pointer(type2, 1);
    node_1 = tree.make_const_node((long double) 0.01);
    tree.set_parent(node_1, node);
    node_1 = tree.make_const_node((long long) 10);
    tree.set_parent(node_1, node);
    node_1 = tree.make_const_node((unsigned long long) 0.01);
    tree.set_parent(node_1, node);
    node_1 = tree.make_const_node((bool) 1);
    tree.set_parent(node_1, node);
    node_1 = tree.make_identifier_node("aa");
    tree.set_parent(node_1, 0);
    tree.set_parent(node, 0);
    // node_1 = tree.make_variable_node(type3, "a", 1);
    //tree.set_parent(node_1, 0);
    node_1 = tree.make_type_specifier_node(ParseType(T_INT));
    tree.set_parent(node_1, 0);
    node_1 = tree.make_declaration_specifier_node();
    node = tree.make_type_specifier_node(ParseType(T_INT));
    tree.set_parent(node, node_1);
    tree.set_parent(node_1, 0);
    printf("1:%s\n", tree.node(node_1)->get_type(&tree).get_info().c_str());
    node_1 = tree.make_declarator_node();
    tree.set_parent(node_1, 0);
    node_1 = tree.make_direct_declarator_node();
    node = tree.make_identifier_node("asd");
    tree.set_parent(node, node_1);
    tree.set_parent(node_1, 0);
    printf("2:%s\n", tree.node(node_1)->get_symbol(&tree).c_str());
    node_1 = tree.make_parameter_list_node();
    tree.set_parent(node_1, 0);
    node_1 = tree.make_parameter_declaration(ParseVariable(ParseType(T_INT), "ass"));
    tree.set_parent(node_1, 0);
    tree.print();
    PType::print_all_type();
}

void test_make_expression() {
    ParseDeclaration::declaration("a", ParseVariable(ParseType(T_INT), "a"));
    ParseDeclaration::declaration("b", ParseVariable(ParseType(T_INT), "b"));
    ParseExpression undefined;
    ParseTree tree("program");
    size_t n1 = tree.make_expression_node(
            ParseExpression(ParseDeclaration::get_variable_declaration("a"))
    ), n2 = tree.make_expression_node(
            ParseExpression(ParseConstant((long long) 10)) / undefined
    ), n3 = tree.make_expression_node(
            ParseExpression(ParseDeclaration::get_variable_declaration("b"))
    ), n4 = tree.make_expression_node(
            tree.node(n3)->get_expression(&tree) + tree.node(n2)->get_expression(&tree)
    );
}


int main() {
    //test_parse_type();
    //test_make_node();
    test_make_expression();
    return 0;
}