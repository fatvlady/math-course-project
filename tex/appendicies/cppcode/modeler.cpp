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

struct model
{
  
  enum behaviour_type {
    place_left,
    place_right,
    place_center,
    place_uniform,
    place_normal
  };

  const static std::map<std::string, behaviour_type> converter;    
  const static std::map<behaviour_type, std::string> back_converter;
  const static double eps;
  //typedef boost::random::uniform_real_distribution<double> uni_d_type;
  typedef std::uniform_real_distribution<double> uni_d_type;

  size_t repeat_count = 100;
  double parking_length = 1.0;

  std::vector<std::pair<double, behaviour_type>> strategy;
  void load(const std::string& path);
  void save(const std::string& path);

  int fit();
  inline double fit_avg();
  void fit_avg_regularized(std::ostream&);

private:
  void split_last(behaviour_type, std::vector<double>&);
  uni_d_type uni_d{0,1};
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator{seed};
};

const double model::eps = 1e-7;
const std::map<std::string, model::behaviour_type> model::converter = 
  {
    {"left", model::place_left},
    {"right", model::place_right},
    {"center", model::place_center},
    {"uniform", model::place_uniform},
    {"normal", model::place_normal}
  };
const std::map<model::behaviour_type, std::string> model::back_converter = 
  {
    {model::place_left, "left"},
    {model::place_right, "right"},
    {model::place_center, "center"},
    {model::place_uniform, "uniform"},
    {model::place_normal, "normal"}
  };

void model::split_last(behaviour_type b, std::vector<double>& v)
{
  double len = v.back();
  v.pop_back();
  switch(b) {
    case place_left:
    case place_right:
      if (len >= 2)
        v.push_back(len - 1);
      break;
    case place_center:
      if (len >= 3)
        v.insert(v.end(), 2, (len - 1) / 2);
      break;
    case place_uniform:
      if (len >= 2) {
        double pos = uni_d_type(0, len - 1)(generator);
        double residuals[2] = {pos, len - 1 - pos};
        for (auto resid : residuals)
          if (resid >= 1) {
            v.push_back(resid);
          }
      }
      break;
    case place_normal:
      throw(std::runtime_error("Not implemented"));
      break;
  }
}

int model::fit()
{
  if (parking_length < 1)
    return 0;
  std::vector<double> lengths{parking_length};
  int counter = 0;
  while(!lengths.empty())
  {
    double acc = 0, p = uni_d(generator);
    for (auto& entry : strategy)
    {
      acc += entry.first;
      if (p < acc) {
        split_last(entry.second, lengths);
        counter++;
        break;
      }
    }
  }
  return counter;
}

inline double model::fit_avg()
{
  double sum = 0.0;
  for (int i = 0; i < repeat_count; i++)
    sum += fit();
  return sum / repeat_count;
}

void model::fit_avg_regularized(std::ostream& out)
{
  out << "Filled ";
  double result = 0;
  if (parking_length < 1)
    out << result;
  else
    out << (result = fit_avg());
  out << "/" << parking_length << " with ratio: ";
  if (parking_length != 0) 
    out << result / parking_length;
  else
    out << "inf";
  out << '.' << std::endl;
}

void model::load(const std::string& path)
{
  strategy.clear();
  using boost::property_tree::ptree;
  ptree pt;
  read_xml(path, pt);
  repeat_count = pt.get<size_t>("model.<xmlattr>.repeat_count", repeat_count);
  parking_length = pt.get<double>("model.<xmlattr>.parking_length", parking_length);
  std::cout << "Loaded parking_length: " << parking_length << std::endl;
  std::cout << "Loaded repeat_count: " << repeat_count << std::endl;
  for (auto& entry : pt.get_child("model"))
  {
    if (entry.first == "<xmlattr>")
      continue;
    strategy.emplace_back(entry.second.get<double>("<xmlattr>.p"), converter.at(entry.first));
    std::cout << "Added " << entry.first << " behaviour with probability " << strategy.back().first << std::endl;
  }
  auto sum = std::accumulate(strategy.begin(), strategy.end(), 0.0, [](double acc, std::pair<double, behaviour_type> next) {return acc + next.first;});
  assert(std::abs(sum - 1.0) < eps);
}

void model::save(const std::string& path)
{
  using boost::property_tree::ptree;
  ptree pt;
  pt.put("model.<xmlattr>.repeat_count", repeat_count);
  pt.put("model.<xmlattr>.parking_length", parking_length);
  for (auto& entry : strategy)
  {
    pt.put("model." + back_converter.at(entry.second) + ".<xmlattr>.p", entry.first);
  }
  write_xml(path, pt);
}

int main(int argc, char* argv[]) {
  model m;
  if (argc > 1)
    m.load(argv[1]);
  else
    m.load("base_model.xml");
  m.fit_avg_regularized(std::cout);
  return 0;
}
