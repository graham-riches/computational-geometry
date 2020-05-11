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
#include "JsonCGALTypes.h"


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ setup dll export ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#define CONVEX_HULL_DLL_EXPORT extern "C" __declspec(dllexport)

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ constants ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
constexpr auto EXPECTED_ARGS = 3;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ function prototypes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void sort_points_2( CGAL_list<JsonCGAL::Point_2d> *points );
bool is_point_less_than_2( JsonCGAL::Point_2d p, JsonCGAL::Point_2d q );
bool makes_right_turn(JsonCGAL::Point_2d p, JsonCGAL::Point_2d q, JsonCGAL::Point_2d m );
CGAL_list< JsonCGAL::Point_2d> run_convex_hull(CGAL_list< JsonCGAL::Point_2d> points );

/* define the main extern DLL function that will be available for external use via the Json API
   NOTE: for usage with python ctypes, this must only use C types (aka no std::string)
*/
CONVEX_HULL_DLL_EXPORT void convex_hull(const char *input, char *output);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ function definitions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/**
 * @brief sort a vector of 2D points
 * 
 * @param points, a pointer to the list of points to be sorted
 * @return 
 */
void sort_points_2(CGAL_list< JsonCGAL::Point_2d> *points )
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
bool is_point_less_than_2( JsonCGAL::Point_2d p, JsonCGAL::Point_2d q )
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
bool makes_right_turn(JsonCGAL::Point_2d p, JsonCGAL::Point_2d q, JsonCGAL::Point_2d m )
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
CGAL_list<JsonCGAL::Point_2d> run_convex_hull( CGAL_list<JsonCGAL::Point_2d> points )
{
	CGAL_list<JsonCGAL::Point_2d> upper_hull;
	CGAL_list<JsonCGAL::Point_2d> lower_hull;
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

    for ( CGAL_list<JsonCGAL::Point_2d>::iterator p = lower_hull.begin(); p < lower_hull.end(); p ++ )
    {
        upper_hull.push_back( *p );
    }
    return upper_hull;
}


/**
 * @brief compute the convex hull of a set of 2D points. Exposed function via DLL export
 *
 * @param input, a pointer to a string buffer of data encoded as JsonCGAL
 * @param output, a pointer to a string buffer of vertex data encoded as JsonCGAL
 */
CONVEX_HULL_DLL_EXPORT void convex_hull(const char *input, char *output)
{
   if ((input == NULL) || (output == NULL))
   {
      return;
   }
   JsonCGAL::JsonCGAL data;
   JsonCGAL::JsonCGAL output_data;
   data.load_from_string(input);
   CGAL_list<JsonCGAL::Point_2d> input_points = data.get_objects<JsonCGAL::Point_2d>(JsonCGAL::Point_2d());
   CGAL_list<JsonCGAL::Point_2d> vertices = run_convex_hull(input_points);
   output_data.add_objects(vertices);
   std::string output_str = output_data.dump_to_string();
   memcpy(output, output_str.c_str(), output_str.size());
}
