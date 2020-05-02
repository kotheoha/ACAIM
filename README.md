# ACAIM

Here, is presented the training process of my impending to publish project/paper **Adaptive Content-Aware Influence Maximization through Online Stochastic Ranking** in the "IEEE Transactions on Knowledge and Data Engineering" - **TKDE** journal. In short, that project is mentioned as **ACAIM**, since it introduces and studies the novel ACAIM problem.

I stress that the input files that I provide here to run the training in ACAIM are random and tiny VK (https://VK.com) samples compared to the ones used in TKDE version. There, I deal with millions of VK data. Here, I just provide these tiny samples to help one to follow the flow of training by understanding the format of input and output files.

The output of training process is used as input to my testing model for the learning evaluation of ACAIM. The code related with the testing process of ACAIM is not provided here for impending to publication reasons in TKDE journal. Last, note that the VK training years I cosider here are 2012 - 2017 (2006 - 2017 in TKDE version), while the VK testing years are 2018 - 2019.

## Input Files

`input_files/friends.txt` (row-format: `user_id: friend_id_1,friend_id_2,...,friend_id_n,`)
* It contains info for the friends that each user has in VK; `n` denotes the last friend of each user.

`input_files/followees.txt` (row-format: `user_id: followee_id_1,followee_id_2,...,followee_id_n,`)
* It contains info for the followees that each user has in VK; `n` denotes the last followee of each user.

`input_files/categories.txt` (row-format: `category_name`)
* It contains the names of the VK categories in which the published posts belong to.

`input_files/posts/category_name/index_category_name.txt` (row-format: `id: n1 posts: n2 max_likes_per_post: n3`)
* It contains info for the 10 most popular features (social network pages) belonging to `category_name`. Namely, for a specific row, `n1` is the id of the respective feature, `n2` is the total number of posts that `n1` published in VK years 2006 - 2019, and `n3` is the maximum number of likes that one of the previously mentioned posts has collected.

`input_files/posts/category_name/posts_agg/posts_agg_id.txt` (three-row-format: `post_id date_id number_of_likes`)
* It contains info for all the posts published by feature with `id` in VK years 2006 - 2019. The row format here is aggregated per three rows, meaning that each three-row pair depicts info for a specific post. Namely, for such a post, the first row is the `post_id` of post, the second row is the `date_id` of post, and the third row is the `number_of_likes` that post has collected.

`input_files/user_post_likes/user_post_likes_category_name_year.txt` (row-format: `user_id: post_1,...,post_n`)
* It contains info for the posts that each user liked in VK in `year`, where those posts are published by any of the respective 10 features belonging to `category_name`; `n` denotes the most recent post that each user liked in `year`. Notice that each post (`post_1,...,post_n`) is uniquely respresented as a `date_id|post_id` pair.

## Output Files

The execution of training for ACAIM is presented in file `acaim_training.cc`. All the previously described `.txt` files constitute the input of `acaim_training.cc` and its output is the files `user_features_trained.txt` and `vis_trained.txt`.

`user_features_trained.txt` (row-format: `user_id: feature_id_1|feature_cnt_1,...,feature_id_n|feature_cnt_n`)
* It contains info for the segmentation of features popularity in users. For instance, if a user liked 25 posts in total published by `feature_id_1` in selected VK training years 2012 - 2017, then `feature_cnt_1 = 25`, while if the same user liked respectively 10 posts in total of `feature_id_n`, then `feature_cnt_n = 10`.

`vis_trained.txt` (row-format: `user_id: visibility_percentage`)
* It contains info for the `visibility_percentage` of each user. The `visibility_percentage` of a user is defined as the percentage of total number of friends and followees of user that should like a post so as the post to be visible to the user.

## Execution

To be able one to execute the `acaim_training.cc` code, it needs first to install the **LEMON** graph library as described here:
* for Linux: https://lemon.cs.elte.hu/trac/lemon/wiki/InstallLinux
* for Windows: https://lemon.cs.elte.hu/trac/lemon/wiki/InstallCmake

Then, it needs to compile the `acaim_training.cc` code by typing in a terminal/console `g++ -W -O3 acaim_training.cc -std=c++11 -lemon -o ACAIM_TRAIN`, and then to execute it as `./ACAIM_TRAIN`.
