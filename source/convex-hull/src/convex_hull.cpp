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


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Includes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <CGAL/Point_2.h>
#include <CGAL/Simple_cartesian.h>



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
typedef CGAL::Simple_cartesian<double> Kernel; /* setup the CGAL kernel*/
typedef CGAL::Point_2<Kernel> Point_2; /* convenience 2D point setup */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Constants ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Function Prototypes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void sort_points_2( std::vector<Point_2> *points );
bool is_point_less_than_2( Point_2 p, Point_2 q );
bool makes_right_turn( Point_2 p, Point_2 q, Point_2 m );
std::vector<Point_2> create_points( int size, double lower_limit, double upper_limit );
std::vector<Point_2> convex_hull( std::vector<Point_2> points );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/**
 * @brief sort a vector of 2D points
 * 
 * @param points, a pointer to the list of points to be sorted
 * @return 
 */
void sort_points_2( std::vector<Point_2> *points )
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
bool is_point_less_than_2( Point_2 p, Point_2 q )
{
   return ( p.x() == q.x() ) ? ( p.y() < q.y() ) : ( p.x() < q.x() );
}


/**
 * @brief checks if three points make a right turn
 * 
 * @param p first point
 * @param q second point
 * @param m third point
 * @return true if the points make a right turn
 * @return false is the points make a left turn or are collinear
 */
bool makes_right_turn( Point_2 p, Point_2 q, Point_2 m )
{
   switch ( CGAL::orientation(p, q, m) )
   {
	case CGAL::RIGHT_TURN:
		return true;
		
   /* intentional fallthrough */
	case CGAL::COLLINEAR:	
   case CGAL::LEFT_TURN:
   default:
      return false;
   }
}


/**
 * @brief create a pseudo random list of points to seed the algorithm with
 * @param size number of points
 * @param lower_limit lower bound of the random range
 * @param upper_limit upper bound of the random range
 * @return vector of points
*/
std::vector<Point_2> create_points( int size, double lower_limit, double upper_limit )
{      
   auto random_point = [](double lower, double upper)
   {
      auto random_function = [distribution = std::uniform_real_distribution<double>(lower, upper),
         random_engine = std::mt19937{std::random_device{}()}] () mutable
      {
         return Point_2{distribution(random_engine), distribution(random_engine)};
      };
      return random_function;
   };
   
   std::vector<Point_2> points(size);
   std::generate(points.begin(), points.end(), random_point(lower_limit, upper_limit) );
   return points;
}

/**
 * @brief compute the convex hull of a set of 2D points
 * 
 * @param points, the input list of points
 * @return vector of vertices of the bounding polygon
 */
std::vector<Point_2> convex_hull( std::vector<Point_2> points )
{
   std::vector<Point_2> upper_hull;
   std::vector<Point_2> lower_hull;

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
		   if ( makes_right_turn( upper_hull[upper_hull.size() - 3], upper_hull[upper_hull.size() - 2], upper_hull[upper_hull.size() - 1]) )
         {
             break;
         }
         else
		   {
			   upper_hull.erase(upper_hull.begin() + upper_hull.size() - 2);
		   }            
      }
   }

   /* now do the lower hull half of the algorithm */
   lower_hull.push_back( points[points.size() - 1 ] );
   lower_hull.push_back( points[points.size() - 2 ] );

   for ( int i = points.size() - 3; i >= 0; i-- )
   {
      /* add the points from the tail end */
      lower_hull.push_back( points[i] );
        
      while ( lower_hull.size() > 2 )
      {         
         if ( makes_right_turn(lower_hull[lower_hull.size( ) - 3], lower_hull[lower_hull.size( ) - 2], lower_hull[lower_hull.size( ) - 1] ) )
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

   for ( std::vector<Point_2>::iterator p = lower_hull.begin(); p < lower_hull.end(); p ++ )
   {
      upper_hull.push_back( *p );
   }
   return upper_hull;
}


/**
 * @brief main algorithm function
 * @param argc number of command line arguments
 * @param argv array of arguments
 * @return None
*/
void main( int argc, char *argv[] )
{
   /* seed a vector of random points */
   auto points = create_points( 100, -5, 5 );
   
   auto hull_points = convex_hull( points );

   for ( auto &point : hull_points )
   {
      std::cout << "Hull point: " << point.x( ) << "," << point.y( ) << std::endl;
   }
}