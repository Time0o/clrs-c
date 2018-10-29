#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/range/iterator_range_core.hpp>

#include "gmock/gmock.h"

extern "C" {
#include "elementary_graph.h"
}

using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::UnorderedElementsAre;
using ::testing::UnorderedElementsAreArray;
using ::testing::Values;


class UndirectedGraphTest : public TestWithParam<graph_representation>
{
using UndirectedGraph =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS>;

protected:
    struct UndirectedTestGraph {
        UndirectedTestGraph() {}

        UndirectedTestGraph(UndirectedGraph const &boost_graph,
                            graph_representation repr)
            : boost_graph(boost_graph),
              graph(construct_from_boost(boost_graph, repr)) {}

        UndirectedGraph boost_graph;
        struct graph *graph;
    };

    void SetUp() {
        auto repr = GetParam();

        undirected_test_graphs["bull graph"] =
            UndirectedTestGraph(construct_bull_graph(), repr);

        undirected_test_graphs["wagner graph"] =
            UndirectedTestGraph(construct_wagner_graph(), repr);

        undirected_test_graphs["bidakis cube"] =
            UndirectedTestGraph(construct_bidakis_cube(), repr);
    }

    void TearDown() {
        for (auto &test_graph : undirected_test_graphs)
            graph_free(test_graph.second.graph);
    }

    std::map<std::string, UndirectedTestGraph> undirected_test_graphs;

private:
    static struct graph * construct_from_boost(
        UndirectedGraph const &boost_graph, enum graph_representation repr) {

        struct graph *g = graph_create(
            boost::num_vertices(boost_graph), UNDIRECTED, repr);

        assert(g);

        for (auto edge : boost::make_iterator_range(boost::edges(boost_graph))) {
            std::size_t from =
                static_cast<std::size_t>(boost::source(edge, boost_graph));

            std::size_t to =
                static_cast<std::size_t>(boost::target(edge, boost_graph));

            assert(graph_add_edge(g, from, to, nullptr) == 0);
        }

        return g;
    }

    static UndirectedGraph construct_bull_graph() {
        UndirectedGraph bull_graph;

        auto v1 = boost::add_vertex(bull_graph);
        auto v2 = boost::add_vertex(bull_graph);
        auto v3 = boost::add_vertex(bull_graph);
        auto v4 = boost::add_vertex(bull_graph);
        auto v5 = boost::add_vertex(bull_graph);

        boost::add_edge(v1, v3, bull_graph);
        boost::add_edge(v2, v4, bull_graph);
        boost::add_edge(v3, v4, bull_graph);
        boost::add_edge(v3, v5, bull_graph);
        boost::add_edge(v4, v5, bull_graph);

        return bull_graph;
    }

    static UndirectedGraph construct_wagner_graph() {
        UndirectedGraph wagner_graph;

        auto v1 = boost::add_vertex(wagner_graph);
        auto v2 = boost::add_vertex(wagner_graph);
        auto v3 = boost::add_vertex(wagner_graph);
        auto v4 = boost::add_vertex(wagner_graph);
        auto v5 = boost::add_vertex(wagner_graph);
        auto v6 = boost::add_vertex(wagner_graph);
        auto v7 = boost::add_vertex(wagner_graph);
        auto v8 = boost::add_vertex(wagner_graph);

        boost::add_edge(v1, v5, wagner_graph);
        boost::add_edge(v2, v6, wagner_graph);
        boost::add_edge(v3, v7, wagner_graph);
        boost::add_edge(v4, v8, wagner_graph);

        return wagner_graph;
    }

    static UndirectedGraph construct_bidakis_cube() {
        UndirectedGraph bidakis_cube;

        auto v1 = boost::add_vertex(bidakis_cube);
        auto v2 = boost::add_vertex(bidakis_cube);
        auto v3 = boost::add_vertex(bidakis_cube);
        auto v4 = boost::add_vertex(bidakis_cube);
        auto v5 = boost::add_vertex(bidakis_cube);
        auto v6 = boost::add_vertex(bidakis_cube);
        auto v7 = boost::add_vertex(bidakis_cube);
        auto v8 = boost::add_vertex(bidakis_cube);
        auto v9 = boost::add_vertex(bidakis_cube);
        auto v10 = boost::add_vertex(bidakis_cube);
        auto v11 = boost::add_vertex(bidakis_cube);
        auto v12 = boost::add_vertex(bidakis_cube);

        boost::add_edge(v1, v7, bidakis_cube);
        boost::add_edge(v2, v6, bidakis_cube);
        boost::add_edge(v3, v11, bidakis_cube);
        boost::add_edge(v4, v10, bidakis_cube);
        boost::add_edge(v5, v9, bidakis_cube);
        boost::add_edge(v8, v12, bidakis_cube);

        return bidakis_cube;
    }
};

INSTANTIATE_TEST_CASE_P(RepresentationTypes, UndirectedGraphTest,
                        Values(ADJACENCY_LISTS)); // TODO

TEST_P(UndirectedGraphTest, CanConstructUndirectedGraph)
{
    for (auto const &test_graph : undirected_test_graphs) {
        auto id = test_graph.first;
        auto boost_graph = test_graph.second.boost_graph;
        auto graph = test_graph.second.graph;

        ASSERT_EQ(static_cast<std::size_t>(boost::num_vertices(boost_graph)),
                  graph_num_vertices(graph))
            << "Constructed '" << id << "' has correct number of vertices.";

        ASSERT_EQ(static_cast<std::size_t>(boost::num_edges(boost_graph)),
                  graph_num_edges(graph))
            << "Constructed '" << id << "' has correct number of edges.";

        auto assert_edge = [&](std::size_t from, std::size_t to){
            std::size_t from_queried, to_queried, edge_queried;

            ASSERT_EQ(0, graph_connector(graph, from, to, &edge_queried))
                << "Can query edge from " << from << " to " << to
                << " in '" << id << "'.";

            ASSERT_EQ(0, graph_endpoints(graph, edge_queried,
                                         &from_queried, &to_queried))
                << "Can query endpoints for edge " << edge_queried
                << " in '" << id << "'.";

            std::vector<std::size_t> tmp {from, to};
            EXPECT_THAT(tmp, UnorderedElementsAre(from_queried, to_queried))
                << "Queried edge source and target correct for edge "
                << edge_queried << " in '" << id << "'.";
        };

        for (auto edge : boost::make_iterator_range(boost::edges(boost_graph))) {
            std::size_t from {boost::source(edge, boost_graph)};
            std::size_t to {boost::target(edge, boost_graph)};

            assert_edge(from, to);
            assert_edge(to, from);
        }
    }
}

TEST_P(UndirectedGraphTest, CanFindNeighbours)
{
    for (auto const &test_graph : undirected_test_graphs) {
        auto id = test_graph.first;
        auto boost_graph = test_graph.second.boost_graph;
        auto graph = test_graph.second.graph;

        auto vertices =
            boost::make_iterator_range(boost::vertices(boost_graph));

        for (auto vertex : vertices) {
            auto p = boost::adjacent_vertices(vertex, boost_graph);
            std::vector<std::size_t> neighbours_expected(p.first, p.second);

            std::size_t *neighbours_actual;
            std::size_t num_neighbours_actual;

            ASSERT_EQ(0, graph_neighbours(
                graph, vertex, INGOING_AND_OUTGOING,
                &neighbours_actual, &num_neighbours_actual));

            EXPECT_EQ(neighbours_expected.size(), num_neighbours_actual)
                << "Vertex " << vertex << " has correct number of neighbours"
                << " in '" << id << "'.";

            std::vector<std::size_t> tmp(
                neighbours_actual, neighbours_actual + num_neighbours_actual);

            EXPECT_THAT(neighbours_expected, UnorderedElementsAreArray(tmp))
                << "Vertex " << vertex << " has correct neighbours"
                << " in '" << id << "'.";

            free(neighbours_actual);
        }
    }
}

TEST_P(UndirectedGraphTest, CanFindIncidences)
{
    for (auto const &test_graph : undirected_test_graphs) {
        auto id = test_graph.first;
        auto boost_graph = test_graph.second.boost_graph;
        auto graph = test_graph.second.graph;

        auto vertices =
            boost::make_iterator_range(boost::vertices(boost_graph));

        for (auto vertex : vertices) {
            auto p = boost::out_edges(vertex, boost_graph);

            std::vector<std::size_t> incidences_expected;

            for (auto e = p.first; e != p.second; ++e) {
                size_t from = boost::source(*e, boost_graph);
                size_t to = boost::target(*e, boost_graph);

                size_t edge;
                ASSERT_EQ(0, graph_connector(graph, from, to, &edge))
                    << "Can determine incident edge in boost graph.";

                incidences_expected.push_back(edge);
            }

            std::size_t *incidences_actual;
            std::size_t num_incidences_actual;

            ASSERT_EQ(0, graph_incidences(
                graph, vertex, INGOING_AND_OUTGOING,
                &incidences_actual, &num_incidences_actual));

            EXPECT_EQ(incidences_expected.size(), num_incidences_actual)
                << "Vertex " << vertex << " has correct number of incidences"
                << " in '" << id << "'.";

            std::vector<std::size_t> tmp(
                incidences_actual, incidences_actual + num_incidences_actual);

            EXPECT_THAT(incidences_expected, UnorderedElementsAreArray(tmp))
                << "Vertex " << vertex << " has correct incidences"
                << " in '" << id << "'.";

            free(incidences_actual);
        }
    }
}
