#include <lemon/list_graph.h>
#include <fstream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <iomanip>
#include <chrono>

using namespace lemon;
using namespace std;

//==========================================================//
// ** printGraph(..) ** //
//==========================================================//
void printGraph(const ListDigraph &SG, const map<unsigned, unsigned> &node_TO_user)
{
	cout << "NODES: " << countNodes(SG) << endl;
	for (ListDigraph::NodeIt n(SG); n != INVALID; ++n)
		cout << node_TO_user.at(SG.id(n)) << endl;
	cout << endl << "EDGES: " << countArcs(SG) << endl;
	for (ListDigraph::ArcIt e(SG); e != INVALID; ++e)
		cout << node_TO_user.at(SG.id(SG.source(e))) << " -> " << node_TO_user.at(SG.id(SG.target(e))) << endl;
}

//==========================================================//
// ** printGraph2(..) ** //
//==========================================================//
void printGraph2(const ListDigraph &SG, const map<unsigned, unsigned> &node_TO_user, const ListDigraph::Node given_node)
{
   cout << "OUT-EDGES:" << endl;
	for (ListDigraph::OutArcIt e(SG, given_node); e != INVALID; ++e)
		cout << node_TO_user.at(SG.id(SG.source(e))) << " -> " << node_TO_user.at(SG.id(SG.target(e))) << endl;
	cout << endl << "IN-EDGES:" << endl;
	for (ListDigraph::InArcIt e(SG, given_node); e != INVALID; ++e)
		cout << node_TO_user.at(SG.id(SG.source(e))) << " -> " << node_TO_user.at(SG.id(SG.target(e))) << endl;
}

//==========================================================//
// ** printFeatures(..) ** //
//==========================================================//
void printFeatures(const map<unsigned, vector<pair<unsigned, unsigned>>> &user_features, const string name_file)
{
	fstream f_out;
	f_out.open(name_file, fstream::out);
	for (auto it = user_features.begin(); it != user_features.end(); it++) {
		f_out << "user " << it->first << ", size " << it->second.size() << ": ";
		unsigned write_cnt = it->second.size();
		if (write_cnt == 0) { cout << "Unexpected 'empty size' ERROR for user: " << it->first << endl;  exit(-1); }
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			if (write_cnt == 1) f_out << it2->first << "|" << it2->second;
			else f_out << it2->first << "|" << it2->second << ",";
			write_cnt--;
		}
		f_out << endl;
	}
	f_out.close();
}

//==========================================================//
// ** addFriends(..) ** //
//==========================================================//
void addFriends(ListDigraph &SG, unordered_map<unsigned, unsigned> &user_TO_node, unordered_map<unsigned, unsigned> &node_TO_user,
	             const string &friend_items, const unsigned user_id)
{
	// Init
	stringstream ss;  string delim = ",";  auto start = 0U;  auto end = friend_items.find(delim);
	unsigned friend_id;

	// ** Add all the friends of 'user_id' to 'SG' graph and to 'user_TO_node' & 'node_TO_user' maps except for last friend ** //
	while (end != string::npos) {
		ss << friend_items.substr(start, end - start);  ss >> friend_id;  ss.clear();
		if (user_TO_node.count(friend_id) == 0) {
			user_TO_node[friend_id] = SG.id(SG.addNode());  node_TO_user[user_TO_node[friend_id]] = friend_id;
		}
		SG.addArc(SG.nodeFromId(user_TO_node[user_id]), SG.nodeFromId(user_TO_node[friend_id]));
		
		start = end + delim.length();
		end = friend_items.find(delim, start);
	}

	// ** Add the last friend of 'user_id' to 'SG' graph and to 'user_TO_node' & 'node_TO_user' maps ** //
	ss << friend_items.substr(start, end);  ss >> friend_id;  ss.clear();
	if (user_TO_node.count(friend_id) == 0) {
		user_TO_node[friend_id] = SG.id(SG.addNode());  node_TO_user[user_TO_node[friend_id]] = friend_id;
	}
	SG.addArc(SG.nodeFromId(user_TO_node[user_id]), SG.nodeFromId(user_TO_node[friend_id]));
}

//==========================================================//
// ** addFollowees(..) ** //
//==========================================================//
void addFollowees(ListDigraph &SG, const unordered_map<unsigned, unsigned> &user_TO_node, const string &followee_items, const unsigned user_id)
{
	// Init
	stringstream ss;  string delim = ",";  auto start = 0U;  auto end = followee_items.find(delim);
	unsigned followee_id;

	// ** Add all the followees of 'user_id' to 'SG' graph except for last followee ** //
	while (end != string::npos) {
		ss << followee_items.substr(start, end - start);  ss >> followee_id;  ss.clear();
		SG.addArc(SG.nodeFromId(user_TO_node.at(followee_id)), SG.nodeFromId(user_TO_node.at(user_id)));

		start = end + delim.length();
		end = followee_items.find(delim, start);
	}

	// ** Add the last followee of 'user_id' to 'SG' graph ** //
	ss << followee_items.substr(start, end);  ss >> followee_id;  ss.clear();
	SG.addArc(SG.nodeFromId(user_TO_node.at(followee_id)), SG.nodeFromId(user_TO_node.at(user_id)));
}

//==========================================================//
// ** addPostLikes(..) ** //
//==========================================================//
void addPostLikes(map<pair<unsigned, unsigned>, unordered_set<unsigned>> &user_post_likes_reversed, const string &post_like_items, const unsigned user_id)
{
	// Init
	stringstream ss;  string delim = ",";  auto start = 0U;  auto end = post_like_items.find(delim);
	string post_like_str;  unsigned long pos_1, pos_2;  unsigned date_id, post_id;

	// ** Add all the '<date_id, post_id>' pairs (posts) of 'user_id' to 'user_post_likes_reversed' except for last '<date_id, post_id>' pair (post) ** //
	while (end != string::npos) {
		// take '<date_id, post_id>' pair (post)
		post_like_str = post_like_items.substr(start, end - start);
		pos_1 = 0;  pos_2 = post_like_str.find("|", pos_1);
		if (pos_2 == string::npos) { cout << "Not found '|' ERROR_1 for: " << post_like_str << endl;  exit(-1); }
		ss << post_like_str.substr(pos_1, pos_2 - pos_1);  ss >> date_id;  ss.clear();
		pos_1 = pos_2 + 1;
		ss << post_like_str.substr(pos_1, string::npos);  ss >> post_id;  ss.clear();
		// process '<date_id, post_id>' pair (post)
		user_post_likes_reversed[make_pair(date_id, post_id)].insert(user_id);

		start = end + delim.length();
		end = post_like_items.find(delim, start);
	}

	// ** Add the last '<date_id, post_id>' pair (post) of 'user_id' to 'user_post_likes_reversed' ** //
	// take '<date_id, post_id>' pair (post)
	post_like_str = post_like_items.substr(start, end);
	pos_1 = 0;  pos_2 = post_like_str.find("|", pos_1);
	if (pos_2 == string::npos) { cout << "Not found '|' ERROR_2 for: " << post_like_str << endl;  exit(-1); }
	ss << post_like_str.substr(pos_1, pos_2 - pos_1);  ss >> date_id;  ss.clear();
	pos_1 = pos_2 + 1;
	ss << post_like_str.substr(pos_1, string::npos);  ss >> post_id;  ss.clear();
	// process '<date_id, post_id>' pair (post)
	user_post_likes_reversed[make_pair(date_id, post_id)].insert(user_id);
}

//==========================================================//
// ** main(..) ** //
//==========================================================//
int main()
{
	auto wcts = chrono::system_clock::now();  // Wall time (start)
	clock_t startcputime = clock();           // CPU time (start)

	ListDigraph SG;  // social graph 'SG'
	unordered_map<unsigned, unsigned> user_TO_node;  // 'user_id' TO 'node_id' map
	unordered_map<unsigned, unsigned> node_TO_user;  // 'node_id' TO 'user_id' map

	// map of 'user_id' TO a vector of '<feature_id, feature_counter>' pairs;
	// each time that a 'user_id' likes a post of 'feature_id', 'feature_counter' increases by a value depending on
	// the year in which that post published; if a user did not like any post, she will not be stored in 'user_features'
	map<unsigned, vector<pair<unsigned, unsigned>>>	user_features;

	// map of '<date_id, post_id>' pair (post) TO a set of users liked that pair (post)
	map<pair<unsigned, unsigned>, unordered_set<unsigned>> user_post_likes_reversed;

	// it is formed based on 'vis-policy' of acaim paper (default: vis = "10% of friends cardinality")
	map<unsigned, double> vis;  // 'user_id' TO 'vis_v' map

	// general variables
	char *line_char = new char[20000000];
	string line_str;
	unsigned long pos_1, pos_2;
	stringstream ss;
	unsigned user_id;

	//==========================================================//
	// ** 'Friends' loading ** //
	//==========================================================//
	{
		cout << "'Friends' loading..." << endl;

		fstream f_in;
		f_in.open("input_files/friends.txt", fstream::in);
		if (!f_in.is_open()) { cout << "Opening ERROR for 'friends.txt' file" << endl;  exit(-1); }

		while (1) {
			f_in.getline(line_char, 20000000);
			if (!f_in) break;  // EOF is reached
			line_str = line_char;
			// take 'user_id'
			pos_1 = 0;  pos_2 = line_str.find(":", pos_1);
			if (pos_2 == string::npos) { cout << "Not found ':' ERROR_1 for line: " << line_str << endl;  exit(-1); }
			ss << line_str.substr(pos_1, pos_2 - pos_1);  ss >> user_id;  ss.clear();
			// add 'user_id' to 'SG' graph and to 'user_TO_node' & 'node_TO_user' maps
			if (user_TO_node.count(user_id) == 0) { user_TO_node[user_id] = SG.id(SG.addNode());  node_TO_user[user_TO_node[user_id]] = user_id; }
			// 'user_id' does not have any friend
			pos_1 = line_str.find(",", pos_2);  if (pos_1 == string::npos) continue;
			// 'user_id' has at least one friend
			pos_1 = pos_2 + 2;
			string friend_items = line_str.substr(pos_1, string::npos);
			pos_1 = friend_items.rfind(",");
			if (pos_1 == string::npos) { cout << "Removal last ',' character ERROR_1 for line: " << line_str << endl;  exit(-1); }
			friend_items.erase(pos_1, 1);
			//cout << user_id << ": " << friend_items << endl;
			addFriends(SG, user_TO_node, node_TO_user, friend_items, user_id);
		}
		f_in.close();

		cout << "'Friends' loading...DONE!" << endl << endl;
	}

	#if 0
	{
	   cout << endl << "'user_id' TO 'node_id' MAP" << endl;
	   for (auto it = user_TO_node.begin(); it != user_TO_node.end(); it++)
	   	cout << it->first << " - " << it->second << endl;
	   cout << endl;
	   cout << "'node_id' TO 'user_id' MAP" << endl;
	   for (auto it = node_TO_user.begin(); it != node_TO_user.end(); it++)
	   	cout << it->first << " - " << it->second << endl;
	   cout << endl;
	   printGraph(SG, node_TO_user);  // general
	   printGraph2(SG, node_TO_user, SG.nodeFromId(user_TO_node[143452]));  // for a specific user
   }
	#endif

	//==========================================================//
	// ** 'Followees' loading ** //
	//==========================================================//
	{
		cout << "'Followees' loading..." << endl;

		fstream f_in;
		f_in.open("input_files/followees.txt", fstream::in);
		if (!f_in.is_open()) { cout << "Opening ERROR for 'followees.txt' file" << endl;  exit(-1); }

		while (1) {
			f_in.getline(line_char, 20000000);
			if (!f_in) break;  // EOF is reached
			line_str = line_char;
			// take 'user_id'
			pos_1 = 0;  pos_2 = line_str.find(":", pos_1);
			if (pos_2 == string::npos) { cout << "Not found ':' ERROR_2 for line: " << line_str << endl;  exit(-1); }
			ss << line_str.substr(pos_1, pos_2 - pos_1);  ss >> user_id;  ss.clear();
			// 'user_id' does not have any followee
			pos_1 = line_str.find(",", pos_2);  if (pos_1 == string::npos) continue;
			// 'user_id' has at least one followee
			pos_1 = pos_2 + 2;
			string followee_items = line_str.substr(pos_1, string::npos);
			pos_1 = followee_items.rfind(",");
			if (pos_1 == string::npos) { cout << "Removal last ',' character ERROR_2 for line: " << line_str << endl;  exit(-1); }
			followee_items.erase(pos_1, 1);
			//cout << user_id << ": " << followee_items << endl;
			addFollowees(SG, user_TO_node, followee_items, user_id);
		}
		f_in.close();

		cout << "'Followees' loading...DONE!" << endl << endl;
	}

	#if 0
	{
		printGraph(SG, node_TO_user);
		printGraph2(SG, node_TO_user, SG.nodeFromId(user_TO_node[208125]));  // for a specific user
		printGraph2(SG, node_TO_user, SG.nodeFromId(user_TO_node[497656]));  // for a specific user
	}
	#endif

	cout << "NODES: " << countNodes(SG) << endl;
	cout << "EDGES: " << countArcs(SG) << endl;
	cout << endl;

	//==========================================================//
	// ** CATRID Training ** //
	//==========================================================//
   {
		cout << "CATRID training..." << endl;

		// Initialize 'vis' map by 'vis-policy' weighting
		// (general: 0.1, special_case: 0.5 when exists a very small number of friends per user)
		for (ListDigraph::NodeIt n(SG); n != INVALID; ++n)
			vis[node_TO_user[SG.id(n)]] = 0.1;

		fstream categories_file;
		categories_file.open("input_files/categories.txt", fstream::in);
		if (!categories_file.is_open()) { cout << "Opening ERROR for 'categories.txt' file" << endl;  exit(-1); }

		while (1)
		{
			categories_file.getline(line_char, 20000000);
			if (!categories_file) break;  // EOF is reached
			line_str = line_char;
			string category = line_str;

			cout << "CATEGORY: " << category << endl;
			// it stores the 10 page ids of the pages belonging to current 'category'
			set<unsigned> category_pages;
			// 'page_id' TO 'offset' map; it is used to avoid needless posts examination of previous years
			// by properly iterating the '<date_id, post_id>' pairs (posts) in 'page_posts_published'
			unordered_map<unsigned, unsigned> page_offset;
			// map of 'page_id' TO a set of '<date_id, post_id>' pairs (posts), denoting the posts that 'page_id' published;
			// if a page did not publish any post, that page will not be stored in 'page_posts_published';
			// each 'page_id' belongs to the same current 'category'
			map<unsigned, set<pair<unsigned, unsigned>>> page_posts_published;

			// ** Add the 10 page ids of current category to 'category_pages' ** //
			{
				fstream index_file;
				index_file.open("input_files/posts/" + category + "/index_" + category + ".txt", fstream::in);
				if (!index_file.is_open()) { cout << "Opening ERROR for 'index_file' of category: " << category << endl;  exit(-1); }

				unsigned page_id;  // 'id' of 'index_file'

				while (1) {
					index_file.getline(line_char, 20000000);
					if (!index_file) break;  // EOF is reached
					line_str = line_char;
					// take 'page_id'
					pos_1 = line_str.find("id");
					if (pos_1 == string::npos) { cout << "Not found 'id' ERROR for line: " << line_str << endl;  exit(-1); }
					pos_1 = pos_1 + 4;
					pos_2 = line_str.find("]", pos_1);
					if (pos_2 == string::npos) { cout << "Not found ']' ERROR for line: " << line_str << endl;  exit(-1); }
					ss << line_str.substr(pos_1, pos_2 - pos_1);  ss >> page_id;  ss.clear();
					// add 'page_id' to 'category_pages'
					category_pages.insert(page_id);
				}
				index_file.close();
			}

			// ** Initialize 'page_offset'
			for (auto it = category_pages.begin(); it != category_pages.end(); it++)
				page_offset[*it] = 0;

			// ** Fill the 'page_posts_published' of current category for all its 10 page ids ** //
			{
				for (auto it = category_pages.begin(); it != category_pages.end(); it++) {
					string page_id_str;  ss << *it;  ss >> page_id_str;  ss.clear();
					fstream posts_agg_file;
					posts_agg_file.open("input_files/posts/" + category + "/posts_agg/posts_agg_" + page_id_str + ".txt", fstream::in);
					if (!posts_agg_file.is_open()) { cout << "Opening ERROR for 'posts_agg_file' of page: " << page_id_str << endl;  exit(-1); }
					unsigned lines_cnt = 0;  unsigned date_id, post_id;
					while (1) {
						posts_agg_file.getline(line_char, 20000000);
						if (!posts_agg_file) break;  // EOF is reached
						line_str = line_char;
						lines_cnt++;
						if ((lines_cnt % 3) == 1) { ss << line_str;  ss >> post_id;  ss.clear(); }
						if ((lines_cnt % 3) == 2) {
							ss << line_str;  ss >> date_id;  ss.clear();
							// skip the posts published in testing years 2018, 2019
							if (date_id >= 1514761200) continue;
							page_posts_published[*it].insert(make_pair(date_id, post_id));
						}
					}
					posts_agg_file.close();
				}
			}

			#if 0
			{
				// ** VALIDITY CHECK - Output 'page_posts_published' ** //
				fstream f_out;  string out_file = category + "_pages.txt";
				f_out.open(out_file, fstream::out);
			   for (auto it = page_posts_published.begin(); it != page_posts_published.end(); it++) {
			   	f_out << "page " << it->first << ", size " << it->second.size() << ": ";
			   	unsigned write_cnt = it->second.size();
			   	if (write_cnt == 0) { cout << "Unexpected 'empty size' ERROR for page: " << it->first << endl;  exit(-1); }
			   	for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			   		if (write_cnt == 1) f_out << it2->first << "|" << it2->second;
			   		else f_out << it2->first << "|" << it2->second << ",";
			   		write_cnt--;
			   	}
			   	f_out << endl;
			   }
			   f_out.close();
		   }
			#endif

			for (unsigned year = 2012; year <= 2017; year++)
			{
				// ** Fill 'user_post_likes_reversed' for current 'category' & 'year' ** //
				//=======================================================================//
				string year_str;  ss << year;  ss >> year_str;  ss.clear();
				cout << "  year: " << year << endl;

				fstream f_in;
				string user_post_likes_file = "input_files/user_post_likes/user_post_likes_" + category + "_" + year_str + ".txt";
				f_in.open(user_post_likes_file, fstream::in);
				if (!f_in.is_open()) { cout << "Opening ERROR for file: " << user_post_likes_file << endl;  exit(-1); }

				user_post_likes_reversed.clear();  // remove values from previous year

				while (1) {
					f_in.getline(line_char, 20000000);
					if (!f_in) break;  // EOF is reached
					line_str = line_char;
					// take 'user_id'
					pos_1 = line_str.find("user");
					if (pos_1 == string::npos) { cout << "Not found 'user' ERROR for line: " << line_str << endl;  exit(-1); }
					pos_1 = pos_1 + 5;
					pos_2 = line_str.find(",", pos_1);
					if (pos_2 == string::npos) { cout << "Not found ',' ERROR for line: " << line_str << endl;  exit(-1); }
					ss << line_str.substr(pos_1, pos_2 - pos_1);  ss >> user_id;  ss.clear();
					// take 'post_like_items'
					pos_1 = line_str.find(":", pos_2);
					if (pos_1 == string::npos) { cout << "Not found ':' ERROR for line: " << line_str << endl;  exit(-1); }
					pos_1 = pos_1 + 2;
					string post_like_items = line_str.substr(pos_1, string::npos);
					//cout << "user " << user_id << ": " << post_like_items << endl;
					addPostLikes(user_post_likes_reversed, post_like_items, user_id);
				}
				f_in.close();

				// ** Go to next year ** //
				//=======================//
				if (user_post_likes_reversed.size() == 0) continue;

				#if 0
				{
					// ** VALIDITY CHECK - Output 'user_post_likes_reversed' ** //
					fstream f_out;  string out_file = category + "_posts_" + year_str + ".txt";
					f_out.open(out_file, fstream::out);
					for (auto it = user_post_likes_reversed.begin(); it != user_post_likes_reversed.end(); it++) {
						f_out << "post " << it->first.first << "|" << it->first.second << ", size " << it->second.size() << ": ";
						unsigned write_cnt = it->second.size();
						if (write_cnt == 0) { cout << "Unexpected 'empty size' ERROR for post: " << it->first.first << "|" << it->first.second << endl;  exit(-1); }
						for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
							if (write_cnt == 1) f_out << *it2;
							else f_out << *it2 << ",";
							write_cnt--;
						}
						f_out << endl;
					}
					f_out.close();
				}
				#endif

				// ** Update/Train 'user_features' & 'vis' maps for each post of current 'category' & 'year' ** //
				//==============================================================================================//
				for (auto it = page_posts_published.begin(); it != page_posts_published.end(); it++)
				{
					//cout << "year: " << year << " - page: " << it->first << " - first: " << next(it->second.begin(), page_offset[it->first])->first << endl;
					for (auto it2 = next(it->second.begin(), page_offset[it->first]); it2 != it->second.end(); it2++)
					{
						if ((year == 2012) && (it2->first >= 1356994800)) break;
						else if ((year == 2013) && (it2->first >= 1388530800)) break;
						else if ((year == 2014) && (it2->first >= 1420066800)) break;
						else if ((year == 2015) && (it2->first >= 1451602800)) break;
						else if ((year == 2016) && (it2->first >= 1483225200)) break;
						else if ((year == 2017) && (it2->first >= 1514761200)) {
							cout << "Unexpected ERROR_1 for:" << endl;
							cout << "year: " << year << " - page: " << it->first << " - post: " << it2->first << "|" << it2->second << endl;  exit(-1);
						}
						else page_offset[it->first] = page_offset[it->first] + 1;

						// local variables per 'current_user'
					   unsigned current_user, friend_user, obs_nf_v, numberOfInArcs;
					   bool feature_exist;  // flag that denotes if current clicked feature exists in 'user_features[current_user]'
					   ListDigraph::Node current_node;  // it corresponds to 'current_user'

					   for (auto it3 = user_post_likes_reversed[*it2].begin(); it3 != user_post_likes_reversed[*it2].end(); it3++) {
					   	// ** Process 'current_user' to update 'F_v' ** //
					   	//==============================================//
					   	current_user = *it3;  // get 'current_user' that liked '*it2'
							obs_nf_v = 0;  numberOfInArcs = 0;  feature_exist = 0;  // init for 'current_user'
							//cout << "current_user: " << current_user << endl;
							for (auto it4 = user_features[current_user].begin(); it4 != user_features[current_user].end(); it4++) {
								if (it4->first == it->first) {
									if (year == 2012) it4->second = it4->second + 1;
									else if (year == 2013) it4->second = it4->second + 2;
									else if (year == 2014) it4->second = it4->second + 3;
									else if (year == 2015) it4->second = it4->second + 4;
									else if (year == 2016) it4->second = it4->second + 5;
									else if (year == 2017) it4->second = it4->second + 6;
									feature_exist = 1;  break;
								}
							}
							if (!feature_exist) {
								if (year == 2012) user_features[current_user].push_back(make_pair(it->first, 1));
								else if (year == 2013) user_features[current_user].push_back(make_pair(it->first, 2));
								else if (year == 2014) user_features[current_user].push_back(make_pair(it->first, 3));
								else if (year == 2015) user_features[current_user].push_back(make_pair(it->first, 4));
								else if (year == 2016) user_features[current_user].push_back(make_pair(it->first, 5));
								else if (year == 2017) user_features[current_user].push_back(make_pair(it->first, 6));
							}

							// ** Process each InArc 'friend_user' of 'current_user' to count 'obs_nf_v' ** //
							//==============================================================================//
							current_node = SG.nodeFromId(user_TO_node[current_user]);  // get 'current_node'
							for (ListDigraph::InArcIt e(SG, current_node); e != INVALID; ++e) {
								//cout << node_TO_user[SG.id(SG.source(e))] << " -> " << node_TO_user[SG.id(SG.target(e))] << endl;
								numberOfInArcs++;
								friend_user = node_TO_user[SG.id(SG.source(e))];  // get 'friend_user'
								if (user_post_likes_reversed[*it2].count(friend_user) != 0) {  // 'friend_user' liked '*it2'
									obs_nf_v++;
								   //cout << "friend_user: " << friend_user << endl;
								}
							}

							// ** Update 'vis_v' of 'current_user' based on found 'obs_nf_v' ** //
							//==================================================================//
							double obs_vis_v;
							if (numberOfInArcs == 0) obs_vis_v = 0.0;  // that 'current_user' would be valuable only as a subscriber to testing phase
							else obs_vis_v = (double)obs_nf_v / (double)numberOfInArcs;
							vis[current_user] = (vis[current_user] + obs_vis_v) / 2.0;
							//cout << "obs_nf_v: " << obs_nf_v << " - friends: " << numberOfInArcs << " - obs_vis_v: " << setprecision(15) << obs_vis_v
							//     << " - current_user_vis: " << setprecision(15) << vis[current_user] << endl << endl;
					   }
					}
				}
			}
		}
		categories_file.close();

		cout << "CATRID training...DONE!" << endl << endl;
	}

	cout << "Trained values exporting..." << endl;

	// ** Output 'user_features' ** //
	//==============================//
	printFeatures(user_features, "user_features_trained.txt");

   // ** Output 'vis' ** //
   //====================//
   fstream f_out;
   f_out.open("vis_trained.txt", fstream::out);
   for (auto it = vis.begin(); it != vis.end(); it++)
   	f_out << it->first << ": " << setprecision(15) << it->second << endl;
   f_out.close();

   cout << "Trained values exporting...DONE!" << endl << endl;

   // ** Measure CPU & Wall time ** //
   //===============================//
	// CPU time (stop)
	double cpu_duration = (double)(clock() - startcputime) / (double)CLOCKS_PER_SEC;
	cout << "CPU time: " << cpu_duration << " sec" << endl;
	// Wall time (stop)
	chrono::duration<double> wall_duration = (chrono::system_clock::now() - wcts);
	cout << "Wall time: " << wall_duration.count() << " sec";
}