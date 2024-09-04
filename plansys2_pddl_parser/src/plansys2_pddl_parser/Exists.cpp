
#include "plansys2_pddl_parser/Domain.h"

namespace parser { namespace pddl {

void Exists::PDDLPrint( std::ostream & s, unsigned indent, const TokenStruct< std::string > & ts, const Domain & d ) const {
	tabindent( s, indent );
	s << "( exists ";

	TokenStruct< std::string > fstruct( ts );

	printParams( 0, s, fstruct, d );

	if ( cond ) cond->PDDLPrint( s, indent + 1, fstruct, d );
	else {
		tabindent( s, indent + 1 );
		s << "()";
	}
	s << "\n";
	tabindent( s, indent );
	s << ")";
}

plansys2_msgs::msg::Node::SharedPtr Exists::getTree( plansys2_msgs::msg::Tree & tree, const Domain & d, const std::vector<std::string> & replace ) const {
  plansys2_msgs::msg::Node::SharedPtr node = std::make_shared<plansys2_msgs::msg::Node>();
  node->node_type = plansys2_msgs::msg::Node::EXISTS;
  node->node_id = tree.nodes.size();
  for ( unsigned i = 0; i < params.size(); ++i ) {
      plansys2_msgs::msg::Param param;
      if (i < replace.size() && params[i]<replace.size()) {
        if (params[i] >= 0) {
          // param has a variable value; replace by action-args
          param.name = replace[params[i]];
        }
      } else {
        param.name = "?" + std::to_string(params[i]);
      }
      node->parameters.push_back(param);
  }

  tree.nodes.push_back(*node);
  plansys2_msgs::msg::Node::SharedPtr child = cond->getTree(tree, d, replace);
  tree.nodes[node->node_id].children.push_back(child->node_id);

  return node;
}

void Exists::parse( Stringreader & f, TokenStruct< std::string > & ts, Domain & d ) {
	f.next();
	f.assert_token( "(" );

	TokenStruct< std::string > es = f.parseTypedList( true, d.types );
  params = d.convertTypes( es.types );

  TokenStruct< std::string > estruct( ts );
  for (size_t i = 0; i < params.size(); i++) {
    params[i] += estruct.size();
  }

  estruct.append( es );

	f.next();
	f.assert_token( "(" );
	if ( f.getChar() != ')' ) {
		cond = d.createCondition( f );
		cond->parse( f, estruct, d );
	}
	else ++f.c;

	f.next();
	f.assert_token( ")" );
}

} } // namespaces
