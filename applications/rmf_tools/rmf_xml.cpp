/**
 * Copyright 2007-2011 IMP Inventors. All rights reserved.
 */
#include <IMP/rmf/atom_io.h>
#include <RMF/RootHandle.h>
#include <IMP/internal/graph_utility.h>
#include <sstream>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

std::string input, output;
po::options_description desc("Usage: input.rmf output.xml");
bool help=false;
bool verbose=false;
int frame=0;
void print_help() {
  std::cerr << desc << std::endl;
}

namespace {

  std::string get_as_attribute_name(std::string name) {
    std::vector<char> data(name.begin(), name.end());
    std::vector<char>::iterator cur= data.begin();
    do {
      cur= std::find(cur, data.end(), ' ');
      if (cur== data.end()) {
        break;
      } else {
        *cur='_';
      }
    } while (true);
    return std::string(data.begin(), data.end());
  }

  template <class TypeT>
  bool show_type_data_xml(RMF::NodeHandle nh,
                          RMF::Category kc,
                          bool opened, std::ostream &out) {
    RMF::RootHandle rh= nh.get_root_handle();
    std::vector<RMF::Key<TypeT> > keys= rh.get_keys<TypeT>(kc);
    for (unsigned int i=0; i< keys.size(); ++i) {
      //std::cout << "key " << rh.get_name(keys[i]) << std::endl;
      if (rh.get_is_per_frame(keys[i])) {
        if (frame >=0) {
          if (nh.get_has_value(keys[i], frame)) {
            if (!opened) {
              out << "<" << kc << "\n";
              opened=true;
            }
            out  << get_as_attribute_name(rh.get_name(keys[i])) << "=\"";
            out << nh.get_value(keys[i], frame) << "\"\n";
          }
        } else {
          int skip=-frame;
          std::ostringstream oss;
          bool some=false;
          for (unsigned int j=0; j< rh.get_number_of_frames(keys[i]); j+=skip) {
            if (j != 0) {
              oss << " ";
            }
            if (nh.get_has_value(keys[i], j)) {
              oss << nh.get_value(keys[i], j);
              some=true;
            } else {
              oss << "-";
            }
          }
          if (some) {
            if (!opened) {
              out << "<" << kc << "\n";
              opened=true;
            }
            out << get_as_attribute_name(rh.get_name(keys[i])) << "=\"";
            out << oss.str() << "\"\n";
          } else {
            /*std::cout << "No frames " << rh.get_name(keys[i])
              << " " << rh.get_is_per_frame(keys[i]) << " " << frame
              << " " << nh.get_has_value(keys[i], frame) << std::endl;*/
          }
        }
      } else {
        if (nh.get_has_value(keys[i])) {
          if (!opened) {
            out << "<" << kc << "\n";
            opened=true;
          }
          out  << get_as_attribute_name(rh.get_name(keys[i])) << "=\"";
          out << nh.get_value(keys[i]) << "\"\n";
        }
      }
    }
    return opened;
  }
  void show_data_xml(RMF::NodeHandle nh,
                     RMF::Category kc,
                     std::ostream &out) {
    bool opened=false;
    opened=show_type_data_xml<RMF::IntTraits>(nh, kc, opened, out);
    opened=show_type_data_xml<RMF::FloatTraits>(nh, kc, opened, out);
    opened=show_type_data_xml<RMF::IndexTraits>(nh, kc, opened, out);
    opened=show_type_data_xml<RMF::StringTraits>(nh, kc, opened, out);
    opened=show_type_data_xml<RMF::NodeIDTraits>(nh, kc, opened, out);
    opened=show_type_data_xml<RMF::IndexDataSet2DTraits>(nh, kc, opened, out);
    opened=show_type_data_xml<RMF::FloatDataSet2DTraits>(nh, kc, opened, out);
    if (opened) {
      out << "/>\n";
    }
  }

  void show_xml(RMF::NodeHandle nh,
                const RMF::Categories& cs, std::ostream &out) {
    out << "<node name=\"" << nh.get_name() << "\" id=\""
        << nh.get_id() << "\" "
        << "type=\"" << RMF::get_type_name(nh.get_type())
        << "\"/>\n";
    if (verbose) {
      for (unsigned int i=0; i< cs.size(); ++i) {
        show_data_xml(nh, cs[i], out);
      }
    }
    RMF::NodeHandles children= nh.get_children();
    for (unsigned int i=0; i< children.size(); ++i) {
      out << "<child>\n";
      show_xml(children[i],cs,  out);
      out << "</child>\n";
    }
  }
}


int main(int argc, char **argv) {
  try {
    desc.add_options()
      ("help,h", "Print the contents of an rmf file to the terminal as xml.")
      ("verbose,v", "Include lots of information about each node.")
      ("frame,f", po::value< int >(&frame),
       "Frame to use")
      ("input-file,i", po::value< std::string >(&input),
       "input hdf5 file")
      ("output-file,o", po::value< std::string >(&output),
       "output xml file");
    po::positional_options_description p;
    p.add("input-file", 1);
    p.add("output-file", 1);
    po::variables_map vm;
    po::store(
              po::command_line_parser(argc,
                                      argv).options(desc).positional(p).run(),
              vm);
    po::notify(vm);
    verbose= vm.count("verbose");
    if (vm.count("help") || input.empty()) {
      print_help();
      return 1;
    }
    RMF::RootHandle rh= RMF::open_rmf_file(input);
    std::ofstream out(output.c_str());
    if (!out) {
      std::cerr << "Error opening file " << output << std::endl;
      return 1;
    }
    RMF::Categories cs= rh.get_categories();
    out << "<?xml version=\"1.0\"?>\n";
    out << "<rmf>\n";
    out << "<description>\n";
    out << rh.get_description() <<std::endl;
    out << "</description>\n";
    out << "<path>\n";
    out << input <<std::endl;
    out << "</path>\n";
    show_xml(rh, cs, out);
    if (rh.get_number_of_bonds() >0) {
      out << "<bonds>\n";
      for (unsigned int i=0; i< rh.get_number_of_bonds(); ++i) {
        std::pair<RMF::NodeHandle, RMF::NodeHandle> handles
          = rh.get_bond(i);
        out << "<bond id0=\""<< handles.first.get_id()
            << "\" id1=\"" << handles.second.get_id() << "\"/>\n";
      }
      out << "</bonds>\n";
    }
    out << "</rmf>\n";
    return 0;
  } catch (const IMP::Exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
