#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

inline string rtrim(string str, const string chars)
{
  str.erase(str.find_last_not_of(chars) + 1);
  return str;
}

inline vector<string> csplit(const string &str)
{
  vector<string> ret;

  stringstream ss(str);
  string tmp = "";

  while (getline(ss, tmp, ','))
  {
    ret.push_back(tmp);
  }

  return ret;
}

vector<string> read_file(const string &path, bool trim_comments)
{
  vector<string> ret;

  ifstream file(path);

  if (!file.is_open())
  {
    printf("couldn't open file: %s\n", path.c_str());
    return ret;
  }

  string line;

  while (getline(file, line))
  {
    if (line.length() == 0)
    {
      continue;
    }

    if (trim_comments && line[0] == '#')
    {
      continue;
    }

    if (line.back() == '\r')
    {
      line.pop_back();
    }

    ret.push_back(line);
  }

  file.close();

  return ret;
}

struct Object
{
  Object(bool enabled, const string &item_name, uint16_t item_id,
         uint32_t group, const vector<string> &level_files,
         const string &level_name, const vector<uint32_t> &offsets) :
    enabled(enabled), item_name(item_name), item_id(item_id), group(group),
    level_files(level_files), level_name(level_name), offsets(offsets) {}

  bool enabled;

  string item_name;
  uint16_t item_id;

  uint32_t group;

  vector<string> level_files;
  string level_name;

  vector<uint32_t> offsets;
};

bool patch_data(const Object &obj, uint32_t new_id, uint32_t size,
                const string &data_dir)
{
  if (obj.level_files.size() != obj.offsets.size())
  {
    printf("files/offsets mismatch\n");
    return false;
  }

  for (size_t i = 0; i < obj.level_files.size(); i++)
  {
    ofstream file(data_dir + "\\" + rtrim(obj.level_files[i], " "),
                  ios_base::binary | ofstream::in);

    if (!file.is_open())
    {
      printf("couldn't open file: %s\n", obj.level_files[i].c_str());
      return false;
    }

    file.seekp(obj.offsets[i], ios_base::beg);
    file.write((char*)&new_id, size);
    file.close();
  }

  return true;
}

vector<Object> read_data(const string &path)
{
  vector<Object> objects;
  vector<string> item_data = read_file(path, true);

  for (const auto &line : item_data)
  {
    auto name = line.find("name=");
    auto id = line.find("id=");
    auto group = line.find("group=");
    auto file = line.find("file=");
    auto level = line.find("level=");
    auto offset = line.find("offset=");

    if (name == string::npos || id == string::npos || group == string::npos ||
        file == string::npos || level == string::npos || offset == string::npos)
    {
      printf("skipping malformed entry:\n%s\n", line.c_str());
      continue;
    }

    vector<uint32_t> offsets;
    vector<string> offsets_str = csplit(line.substr(offset + 7, line.length() - offset - 7));

    for (const auto &it : offsets_str)
    {
      offsets.push_back(strtoul(it.c_str(), nullptr, 16));
    }

    objects.emplace_back(
      line[0] == '+',
      line.substr(name + 5, id - name - 5),
      stoi(line.substr(id + 3, group - id - 3)),
      stoi(line.substr(group + 6, file - group - 6)),
      csplit(line.substr(file + 5, level - file - 5)),
      line.substr(level + 6, offset - level - 6),
      offsets);
  }

  return objects;
}

int main()
{
  uint32_t game_index;

  while (true)
  {
    string game_index_str;

    printf("choose game\n1. MM6\n2. MM7\n> ");
    getline(cin, game_index_str);

    if (game_index_str != "")
    {
      game_index = stoi(game_index_str);

      if (game_index == 1 || game_index == 2)
      {
        break;
      }
    }

    printf("invalid index\n");
  }

  string item_data_file;
  string event_data_file;
  string data_dir;
  string game_str;

  switch (game_index)
  {
    case 1:
      item_data_file = "mm6itemdata";
      event_data_file = "mm6eventdata";
      data_dir = "mm6data";
      game_str = "mm6";
      break;
    case 2:
      item_data_file = "mm7itemdata";
      event_data_file = "mm7eventdata";
      data_dir = "mm7data";
      game_str = "mm7";
      break;
    default:
      printf("bad game index: %lu\n", game_index);
      return 1;
  }

  string patched_lods = "";

  printf("are you using grayface's patched .lods? (y/n)\n> ");
  getline(cin, patched_lods);

  if (patched_lods == "y")
  {
    item_data_file += "-patched";
    event_data_file += "-patched";
  }

  item_data_file += ".txt";
  event_data_file += ".txt";

  uint64_t seed;
  string user_seed = "";

  printf("enter a seed (any number between 0 and 2^63, leave blank for random)\n> ");
  getline(cin, user_seed);

  if (user_seed != "")
  {
    seed = strtoull(user_seed.c_str(), nullptr, 10);
  }
  else
  {
    seed = chrono::system_clock::now().time_since_epoch().count();
  }

  const string logfile_name = game_str + "-randomizer-" + to_string(seed) + ".txt";
  ofstream logfile(logfile_name);

  logfile << "seed: " << seed << "\n\n";

  string random_items = "";

  printf("randomize items? (y/n)\n> ");
  getline(cin, random_items);

  if (random_items == "y")
  {
    printf("randomizing items...\n");

    vector<Object> item_objects = read_data(item_data_file);
    vector<Object> disabled_objs;
    vector<Object> enabled_objs;

    for (const auto &it : item_objects)
    {
      it.enabled ? enabled_objs.push_back(it) : disabled_objs.push_back(it);
    }

    vector<Object> group_0_objs;
    vector<Object> group_1_objs;

    for (const auto &it : enabled_objs)
    {
      switch (it.group)
      {
        case 0:
          group_0_objs.push_back(it);
          break;
        case 1:
          group_1_objs.push_back(it);
          break;
        default:
          printf("invalid group index: %lu\n", it.group);
          return 1;
      }
    }

    vector<Object> shuffled_g0_objs = group_0_objs;
    vector<Object> shuffled_g1_objs = group_1_objs;
    vector<Object> event_objects = read_data(event_data_file);

    shuffle(shuffled_g0_objs.begin(), shuffled_g0_objs.end(), mt19937_64(seed));
    shuffle(shuffled_g1_objs.begin(), shuffled_g1_objs.end(), mt19937_64(seed));


    logfile << "--------------------------------------------------------------------------------\n"
            << "map containing item     |  original item            <-  randomized item\n"
            << "--------------------------------------------------------------------------------\n";

    for (size_t i = 0; i < shuffled_g0_objs.size(); i++)
    {
      patch_data(group_0_objs[i], shuffled_g0_objs[i].item_id, 2, data_dir);

      logfile << group_0_objs[i].level_name << "|  "
              << group_0_objs[i].item_name << " <-  "
              << shuffled_g0_objs[i].item_name << "\n";
    }

    for (size_t i = 0; i < shuffled_g1_objs.size(); i++)
    {
      patch_data(group_1_objs[i], shuffled_g1_objs[i].item_id, 2, data_dir);

      logfile << group_1_objs[i].level_name << "|  "
              << group_1_objs[i].item_name << " <-  "
              << shuffled_g1_objs[i].item_name << "\n";
    }

    for (const auto &it : disabled_objs)
    {
      patch_data(it, it.item_id, 2, data_dir);
    }

    logfile << "--------------------------------------------------------------------------------\n\n"
            << "applied the following event fixes:\n";

    for (const auto &it : event_objects)
    {
      logfile << "* " << it.item_name << "\n";
      patch_data(it, it.item_id, 1, data_dir);
    }
  }

  string random_mons = "";

  if (game_str == "mm6")
  {
    printf("randomize monsters? (y/n)\n> ");
    getline(cin, random_mons);
  }

  if (random_mons == "y")
  {
    printf("randomizing monsters...\n");

    const string mons_path = "mm6data/mapstats.txt";

    vector<string> mapstats_txt = read_file(mons_path, false);
    vector<string> left;
    vector<string> right;
    vector<string> header;

    header = mapstats_txt;
    header.resize(3);
    mapstats_txt.erase(mapstats_txt.begin(), mapstats_txt.begin() + 3);

    for (const auto &it : mapstats_txt)
    {
      if (it.length() > 0)
      {
        if (it[0] == '\t')
        {
          continue;
        }
      }

      const auto f = it.find('.');

      if (f == string::npos)
      {
        printf("skipping malformed monster entry:\n%s\n", it.c_str());
        continue;
      }

      left.push_back(it.substr(0, f + 4));
      right.push_back(it.substr(f + 4));
    }

    shuffle(right.begin(), right.begin() + 50, mt19937_64(seed));

    ofstream randomized_mons("mm6data/mapstats.txt");

    for (const auto &it : header)
    {
      randomized_mons << it << endl;
    }

    for (size_t i = 0; i < left.size(); i++)
    {
      randomized_mons << left[i] << right[i] << endl;
    }

    randomized_mons.close();
  }

  logfile.close();

  printf("done! wrote log to: %s\n", logfile_name.c_str());

  system("pause");
  return 0;
}
