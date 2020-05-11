/**
 * @file main.cpp
 * @author Graham Riches (graham.riches@live.com)
 * @brief main executable application
 * @version 0.1
 * @date 2020-04-29
 * 
 * @copyright Copyright (c) 2020
 * 
 */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ includes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "JsonCGAL.h"
#include "cgal_kernel_config.h"
#include "CGAL_Types.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ setup dll export ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define CONVEX_HULL_DLL_EXPORT extern "C" __declspec(dllexport)

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ constants ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
constexpr auto EXPECTED_ARGS = 3;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ function prototypes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void sort_points_2( CGAL_list<JsonCGAL::Point> *points );
bool is_point_less_than_2( JsonCGAL::Point p, JsonCGAL::Point q );
bool makes_right_turn(JsonCGAL::Point p, JsonCGAL::Point q, JsonCGAL::Point m );
CGAL_list< JsonCGAL::Point> convex_hull(CGAL_list< JsonCGAL::Point> points );
CONVEX_HULL_DLL_EXPORT int convex_hull(CGALTypes::Point *points, int size, CGALTypes::Point *vertices);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ function definitions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/**
 * @brief main algorithm function
 */
int main(int argc, char *argv[])
{	
	if ( argc < EXPECTED_ARGS )
	{
		std::cerr << "Invalid number of arguments" << std::endl;
		std::cout << "Usage: Convex_Hull [input_data.json] [output_data.json]" << std::endl;
		return -1;
	}
	/* create the file names */
	std::string inputFile = argv[1];
	std::string outputFile = argv[2];


	/* create a list of points to be read in and one for the vertices of the hull */
	CGAL_list<JsonCGAL::Point> points;
	CGAL_list<JsonCGAL::Point> vertices;
	
	/* read in the data */
	JsonCGAL::JsonCGAL json_data;
	if (!json_data.load(inputFile))
	{
		std::cout << "Failed to read json file" << std::endl;
		return -1;
	}
	points = json_data.get_points();
	
	/* run the algorithm */
	vertices = convex_hull( points );

	/* dump the output */
	json_data.set_points(vertices);
	json_data.dump(outputFile);

	return 0;
}



/**
 * @brief sort a vector of 2D points
 * 
 * @param points, a pointer to the list of points to be sorted
 * @return 
 */
void sort_points_2(CGAL_list< JsonCGAL::Point> *points )
{
    std::sort( points->begin(), points->end(), is_point_less_than_2 );
    return;
}


/**
 * @brief compares two points for use in sorting.
 * 
 * @param p 
 * @param q 
 * @return true if p is less than q
 * @return false if p is greater than q
 * @note because this a 2D sorting function, the default comparison is based
 *       on the x-coordinate of the points. If x is equal, the points are sorted
 *       by y-coordinate next. This will return false if two points are identical.
 */
bool is_point_less_than_2( JsonCGAL::Point p, JsonCGAL::Point q )
{
    return ( p.x() == q.x() ) ? ( p.y() < q.y() ) : ( p.x() < q.x() );
}


/**
 * @brief checks if three points make a right turn
 * 
 * @param p 
 * @param q 
 * @param m 
 * @return true if the points make a right turn
 * @return false is the points make a left turn or are collinear
 */
bool makes_right_turn(JsonCGAL::Point p, JsonCGAL::Point q, JsonCGAL::Point m )
{
    switch ( CGAL::orientation(p, q, m) )
    {
		case CGAL::RIGHT_TURN:
			return true;
		
		case CGAL::COLLINEAR:	
        case CGAL::LEFT_TURN:
        default:
            return false;
    }
}


/**
 * @brief compute the convex hull of a set of 2D points
 * 
 * @param points, the input list of points
 * @return PointsList_t a list of vertices of the bounding polygon
 */
CGAL_list<JsonCGAL::Point> convex_hull( CGAL_list<JsonCGAL::Point> points )
{
	CGAL_list<JsonCGAL::Point> upper_hull;
	CGAL_list<JsonCGAL::Point> lower_hull;
	/* try reserving memory based on the length of the input points as we can't have more hull points than input points 
	   this trades memory for speed. No notable performance gains found.
	*/
	upper_hull.reserve(points.size());
	lower_hull.reserve(points.size());

	int j;

    /* start by sorting the list of points lexicographically */
    sort_points_2( &points );

    /* append the first two points of the sorted list to the upper hull list */
    upper_hull.push_back( points[0] );
    upper_hull.push_back( points[1] );

    /* start the upper hull loop at the third index */
    for ( int i = 2; i < points.size(); i++ )
    {
        /* add the newest point to the hull */
        upper_hull.push_back( points[i] );

        /* remove points if they are not valid */
        while ( upper_hull.size() > 2 )
        {
			j = upper_hull.size();
			if ( makes_right_turn(upper_hull[j - 3], upper_hull[j - 2], upper_hull[j - 1]) )
            {
                break;
            }
            else
			{
				upper_hull.erase(upper_hull.begin() + j - 2);
			}            
        }
    }

    /* now do the lower hull half of the algorithm */
    lower_hull.push_back( points[points.size() - 1 ] );
    lower_hull.push_back( points[points.size() - 2 ] );
    for ( int i = points.size()-3; i >= 0; i-- )
    {
        /* add the points from the tail end */
        lower_hull.push_back( points[i] );
        
        while ( lower_hull.size() > 2 )
        {
            j = lower_hull.size();
            if ( makes_right_turn(lower_hull[j-3], lower_hull[j-2], lower_hull[j-1] ) )
            {
                break;
            }
            else
            {
                lower_hull.erase(lower_hull.end() - 2 );
            }
        }
    }    

    /* remove the first and last point in lower_hull (already in upper_hull) and append the two lists */
    lower_hull.pop_back();
    lower_hull.erase( lower_hull.begin() );

    for ( CGAL_list<JsonCGAL::Point>::iterator p = lower_hull.begin(); p < lower_hull.end(); p ++ )
    {
        upper_hull.push_back( *p );
    }
    return upper_hull;
}



CONVEX_HULL_DLL_EXPORT int convex_hull(CGALTypes::Point *points, int size, CGALTypes::Point *vertices)
{
	if ((points == NULL) || (vertices == NULL))
	{
		return 0;
	}
	JsonCGAL::JsonCGAL json_data;
	json_data.set_points(points, size);
	CGAL_list<JsonCGAL::Point> points_v = json_data.get_points();
	CGAL_list<JsonCGAL::Point> vertices_v = convex_hull(points_v);

	for (int i = 0; i < vertices_v.size(); i++)
	{
		vertices->x = vertices_v[i].x();
		vertices->y = vertices_v[i].y();
		vertices++;
	}
	return static_cast<int>(vertices_v.size());
}