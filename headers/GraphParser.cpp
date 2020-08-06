#include "GraphParser.h"

namespace mtm
{
    void GraphParser::vertexList(bool get_next_token)
    {
        if(get_next_token)
        {
            lexer.fetchNextToken();
        }
        switch(lexer.getCurrentToken().type)
        {
            case Type::variable:
            {
                vertices.insert(lexer.getCurrentToken().name);
                lexer.fetchNextToken();
                if(lexer.getCurrentToken().type == Type::comma)
                {
                    vertexList(true);
                }
                break;
            }
            default:
            {
                throw VertexSyntaxError();
            }
        }
    }

    void GraphParser::edge(bool get_next_token)
    {
        if(get_next_token)
        {
            lexer.fetchNextToken();
        }
        switch(lexer.getCurrentToken().type)
        {
            case Type::abrac_open:
            {
                if(lexer.fetchNextToken().type != Type::variable)
                {
                    throw IllegalEdgeName();
                }
                if(vertices.count(lexer.getCurrentToken().name) == 0)
                {
                    throw VertexInEdgeDoesNotExist();
                }
                std::pair<std::string, std::string> pair;
                pair.first = lexer.getCurrentToken().name; //Save first vertex
                if(lexer.fetchNextToken().type != Type::comma)
                {
                    throw IllegalEdgeName();
                }
                if(lexer.fetchNextToken().type != Type::variable)
                {
                    throw IllegalEdgeName();
                }
                if(vertices.count(lexer.getCurrentToken().name) == 0)
                {
                    throw VertexInEdgeDoesNotExist();
                }
                pair.second = lexer.getCurrentToken().name; //Save second vertex
                if(lexer.fetchNextToken().type != Type::abrac_close)
                {
                    throw IllegalEdgeName();
                }
                if(pair.first == pair.second || edges.count(pair) != 0)
                {
                    throw ParallelSelfEdgeDetected();
                }
                edges.insert(pair); //Insert edge

                lexer.fetchNextToken(); //Eat the '>' token.
                return;
            }
            default:
            {
                throw EdgeSyntaxError();
            }
        }
    } //End of function edge()

    void GraphParser::edgeList(bool get_next_token)
    {
        edge(get_next_token);
        lexer.fetchNextToken();
        switch(lexer.getCurrentToken().type)
        {
            case Type::comma:
                edgeList(true);
                break;
            default:
                throw EdgeSyntaxError();
        }
    }

    void GraphParser::evaluate()
    {
        lexer.fetchNextToken();
        switch(lexer.getCurrentToken().type)
        {
            case Type::end: case Type::eof:
            {
                return;
            }
            case Type::line:
            {
                lexer.fetchNextToken();
                if(lexer.getCurrentToken().type != Type::end
                || lexer.getCurrentToken().type != Type::eof)
                {
                    throw IllegalGraphLiteral();
                }
                return;
            }
            case Type::variable:
            {
                vertexList(false);
                switch(lexer.getCurrentToken().type)
                {
                    case Type::end: case Type::eof:
                    {
                        return;
                    }
                    case Type::line:
                    {
                        lexer.fetchNextToken();
                        switch(lexer.getCurrentToken().type)
                        {
                            case Type::end: case Type::eof:
                            {
                                return;
                            }
                            case Type::abrac_open:
                            {
                                edgeList(false);
                                return;
                            }
                            default:
                            {
                                throw EdgeSyntaxError();
                            }
                        }
                        return;
                    }
                    default:
                    {
                        throw VertexSyntaxError();
                    }
                }
            }
        }
    }
}