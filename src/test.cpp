#include "ParseTree.hpp"
#include <iostream>
#include "BaseBlock.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
bool generating_code = true;

void expr_call_back(const ParseExpression &expression) {

}

void set_node_begen_code(size_t node_id, size_t code_id) {

}

using namespace std;

void test_parse_type() {
    typedef ParseType PType;
    PType type1(T_INT), type2 = PType::get_pointer(type1, 1), type3 = PType::get_pointer(type2, 1);
    vector<pair<string, size_t> > fields;
    fields.emplace_back("a", type1.get_id());
    fields.emplace_back("b", type2.get_id());
    fields.emplace_back("c", type3.get_id());
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
    try {
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
        node_1 = tree.make_declarator_node();
        tree.set_parent(node_1, 0);
        node_1 = tree.make_direct_declarator_node();
        node = tree.make_identifier_node("asd");
        tree.set_parent(node, node_1);
        tree.set_parent(node_1, 0);
        node_1 = tree.make_parameter_list_node();
        tree.set_parent(node_1, 0);
        node_1 = tree.make_parameter_declaration(ParseVariable(ParseType(T_INT), "ass"));
        tree.set_parent(node_1, 0);
        tree.print();
        PType::print_all_type();
    } catch (ParseException &exc) {
        cout << exc.get_info() << endl;
        tree.print();
    }

}

void test_make_expression() {
    ParseScope::get_scope(0).declaration("a", ParseVariable(ParseType(T_INT), "a"));
    ParseScope::get_scope(0).declaration("b", ParseVariable(ParseType(T_INT), "b"));
    ParseExpression undefined;
    ParseTree tree("program");
    size_t n1 = tree.make_expression_node(
            ParseExpression(ParseScope::get_scope(0).get_variable_declaration("a"))
    ), n2 = tree.make_expression_node(
            ParseExpression(ParseConstant((long long) 10)) / undefined
    ), n3 = tree.make_expression_node(
            ParseExpression(ParseScope::get_scope(0).get_variable_declaration("b"))
    ), n4 = tree.make_expression_node(
            tree.node(n3).get_expression() + tree.node(n2).get_expression()
    );
}

void test_scope() {
    ParseTree tree("program");
    ParseScope::new_scope(0);
    ParseScope::new_scope(1);
    ParseScope::new_scope(2);
    ParseScope::new_scope(0);
    ParseScope::new_scope(0);
    ParseScope::new_scope(2);
    ParseScope::new_scope(3);
    ParseScope::print_all_declaration();
}

void test_expression() {
    try {
        ParseVariable a(ParseType(T_INT, S_LONG), "a"), b(ParseType(T_LONG, S_LONG), "b");
        ParseExpression
//                e1(ParseConstant((long long) 1)),
//                e2(ParseConstant((long long) 2)),
//                e3(ParseExpression::get_logic_expression(E_GE, e1, e2)),
                e4(a), e5(b), e6(e4 + e5),
                e7(ParseExpression::get_assign_expression(e4, e5));
        ParseExpression::print_all_expression();
    } catch (ParseException &exc) {
        cout << exc.get_info() << endl;
        ParseExpression::print_all_expression();
    }
}

void test_base_block() {
    vector<Qua> tests;
    tests.emplace_back("1", "_", "_", "0");
    tests.emplace_back("2", "_", "_", "1");
    tests.emplace_back("3", "_", "_", "2");
    tests.emplace_back("4", "_", "_", "2");
    tests.emplace_back("5", "_", "_", "2");
    tests.emplace_back("6", "_", "_", "2");
    tests.emplace_back("7", "_", "_", "2");
    tests.emplace_back("8", "_", "_", "2");
    tests.emplace_back("j<", "_", "_", "2");
    tests.emplace_back("10", "_", "_", "2");
    tests.emplace_back("j<", "_", "_", "1");
    tests.emplace_back("12", "_", "_", "4");
    tests.emplace_back("13", "_", "_", "5");
    tests.emplace_back("14", "_", "_", "5");
    tests.emplace_back("15", "_", "_", "5");
    tests.emplace_back("16", "_", "_", "5");
    tests.emplace_back("j<", "_", "_", "12");
}

void test_struct() {
    ParseType type(T_INT);
    vector< pair<string, size_t> > s_t;
    s_t.emplace_back("a", type.get_id());
    s_t.emplace_back("b", type.get_id());
    ParseType struct_type = ParseType::get_struct(s_t);
    cout << type.get_info() << endl;
    cout << struct_type.get_info() << endl;
    ParseType::print_all_type();
}

int main() {
    //test_parse_type();
    //test_make_node();
    //test_make_expression();
    //test_scope();
    //test_expression();
    //test_base_block();
    test_struct();
    return 0;
}

#pragma clang diagnostic pop