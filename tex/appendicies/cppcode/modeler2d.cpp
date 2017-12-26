#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/random/uniform_real_distribution.hpp>
//#include <boost/random/normal_distribution.hpp>
#include <random>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <chrono>
#include <list>
#include <tuple>


struct model2d
{
    double a;
    double b;
    // only uniform distribution is supported here

    struct point
    {
        point(double x, double y) : x(x), y(y) { }
        double x;
        double y;
    };

    typedef std::pair<point, point> rectangle;
    typedef std::uniform_real_distribution<double> uni_d_type;
  
    int fit();
    inline double fit_avg();
    void fit_avg_regularized(std::ostream& out);
    
    model2d(int a, int b, size_t repeat_count = 100) : a(a), b(b), repeat_count(repeat_count) { }
    
private:
    size_t repeat_count;
    double eps = 1e-6;
    double rec_square(const rectangle&);
    void consume(std::list<rectangle>& recs, const rectangle& exclusion);
    bool does_intersect(const rectangle&, const rectangle&);
    double current_square;
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator{seed};
};

double model2d::rec_square(const rectangle& rec) {
    return (rec.second.x - rec.first.x) * (rec.second.y - rec.first.y);
}

bool model2d::does_intersect(const rectangle& left, const rectangle& right)
{
    return std::max(left.first.x, right.first.x) < std::min(left.second.x, right.second.x) &&
    std::max(left.first.y, right.first.y) < std::min(left.second.y, right.second.y);
}

void model2d::consume(std::list<rectangle>& recs, const rectangle& exclusion)
{
    auto g_iterator = recs.begin();
    std::list<rectangle> additions;
    
    //std::cout << "Exclusion(" << exclusion.first.x << "," << exclusion.first.y << "," <<
    //exclusion.second.x << "," << exclusion.second.y << ")" << std::endl;
    while (g_iterator != recs.end())
    {
        auto iterator = g_iterator++;
        if (does_intersect(*iterator, exclusion)) {
            // DO SOME EPIC STUFF HERE
            std::vector<std::pair<double,double>> x_sides, y_sides;
            if (exclusion.first.x > iterator->first.x && exclusion.second.x < iterator->second.x)
            {
                x_sides.push_back(std::make_pair(iterator->first.x, exclusion.first.x));
                x_sides.push_back(std::make_pair(exclusion.first.x, exclusion.second.x));
                x_sides.push_back(std::make_pair(exclusion.second.x, iterator->second.x));
            }
            else if (exclusion.first.x > iterator->first.x && exclusion.first.x < iterator->second.x)
            {
                x_sides.push_back(std::make_pair(iterator->first.x, exclusion.first.x));
                x_sides.push_back(std::make_pair(exclusion.first.x, iterator->second.x));
            }
            else if (exclusion.second.x > iterator->first.x && exclusion.second.x < iterator->second.x)
            {
                x_sides.push_back(std::make_pair(iterator->first.x, exclusion.second.x));
                x_sides.push_back(std::make_pair(exclusion.second.x, iterator->second.x));
            }
            else
            {
                x_sides.push_back(std::make_pair(iterator->first.x, iterator->second.x));
            }
            
            if (exclusion.first.y > iterator->first.y && exclusion.second.y < iterator->second.y)
            {
                y_sides.push_back(std::make_pair(iterator->first.y, exclusion.first.y));
                y_sides.push_back(std::make_pair(exclusion.first.y, exclusion.second.y));
                y_sides.push_back(std::make_pair(exclusion.second.y, iterator->second.y));
            }
            else if (exclusion.first.y > iterator->first.y && exclusion.first.y < iterator->second.y)
            {
                y_sides.push_back(std::make_pair(iterator->first.y, exclusion.first.y));
                y_sides.push_back(std::make_pair(exclusion.first.y, iterator->second.y));
            }
            else if (exclusion.second.y > iterator->first.y && exclusion.second.y < iterator->second.y)
            {
                y_sides.push_back(std::make_pair(iterator->first.y, exclusion.second.y));
                y_sides.push_back(std::make_pair(exclusion.second.y, iterator->second.y));
            }
            else
            {
                y_sides.push_back(std::make_pair(iterator->first.y, iterator->second.y));
            }
            
            for (auto& x_side : x_sides)
                for (auto& y_side : y_sides) {
                    rectangle new_rec = std::make_pair(point(x_side.first, y_side.first), point(x_side.second, y_side.second));
                    if (!does_intersect(new_rec, exclusion))
                        additions.push_back(new_rec);
                    else
                        current_square -= rec_square(new_rec);
                    assert(rec_square(new_rec) >= 0);
                }
            recs.erase(iterator);
        }
    }
    recs.splice(recs.end(), additions);
}

int model2d::fit()
{
    if (a < 1 || b < 1)
        return 0;
    std::list<rectangle> rectangles{{{0,0}, {a,b}}};
    current_square = a * b;
    int counter = 0;
    while(!rectangles.empty() && !(current_square <= eps))
    {
        auto decision = uni_d_type(0, current_square)(generator);
        auto current_rect = std::find_if(rectangles.begin(), rectangles.end(), [decision](rectangle& rec) {
            static double sum = 0;
            sum += (rec.second.x - rec.first.x) * (rec.second.y - rec.first.y);
            if (sum >= decision)
                return true;
            return false;
        });
        auto x = uni_d_type(current_rect->first.x, current_rect->second.x)(generator);
        auto y = uni_d_type(current_rect->first.y, current_rect->second.y)(generator);
        rectangle exclusion = std::make_pair(point(x - 1, y - 1), point(x + 1, y + 1));
        consume(rectangles, exclusion);
        counter++;
    }
    return counter;
}

inline double model2d::fit_avg()
{
    double sum = 0.0;
    for (int i = 0; i < repeat_count; i++)
        sum += fit();
    return sum / repeat_count;
}

void model2d::fit_avg_regularized(std::ostream& out)
{
    out << "Filled ";
    double result = 0;
    if (a < 1 || b < 1)
        out << result;
    else
        out << (result = fit_avg());
    out << "/" << a*b << " with ratio: ";
    if (a * b != 0)
        out << result / (a * b);
    else
        out << "inf";
    out << '.' << std::endl;
}


int main()
{
    model2d m {100, 100};
    m.fit_avg_regularized(std::cout);
    return 0;
}
