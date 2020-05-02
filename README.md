# ACAIM

Here, is presented the training process of my impending to publish project/paper **Adaptive Content-Aware Influence Maximization through Online Stochastic Ranking** in the "IEEE Transactions on Knowledge and Data Engineering" - **TKDE** journal. In short, that project is mentioned as **ACAIM**, since it introduces and studies the novel ACAIM problem.

I stress that the input files that I provide here to run the training in ACAIM are tiny VK (https://VK.com) samples compared to the ones used in TKDE version. There, I deal with millions of VK data. Here, I just provide these tiny samples to show the flow of training and the format of input files. So, no efforts to understand the semantics of training output should be made here.

## Format of Input Files

`friends.txt` (row-format: `user_id: friend_id_1,friend_id_2,...,friend_id_n,`)
* It contains info for the friends that each user has in VK; `n` denotes the last friend of each user.

`followees.txt` (row-format: `user_id: followee_id_1,followee_id_2,...,followee_n,`)
* It contains info for the followees that each user has in VK; `n` denotes the last followee of each user.

`categories.txt` (row-format: `category_name`)
* It contains the names of the VK categories in which the published posts belong to.

`input_files/posts/category_name/index_category_name.txt` (row-format: `id: n1 posts: n2 max_likes_per_post: n3`)
* It contains info for the 10 most popular features (*social network pages*) belonging to `category_name`. Namely, for a specific row, `n1` is the id of the respective feature, `n2` is the total number of posts that `n1` published in VK years 2006-2019, and `n3` is the maximum number of likes that one of the previously mentioned posts has collected.
