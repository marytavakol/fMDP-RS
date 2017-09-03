fMDP-RS
=======

Recommender Systems using factored MDP

Data:

The log data is in the following format:

Item_ID	ItemFamily_ID	Brand	Color	CategoryGroup1	CategoryGroup2	CategoryGroup3	CategoryGroup4	CategoryGroup5	Cookie_ID	Session_ID	Action	Count	PriceLeve

The lines are sorted based on Session_ID preserving the original order, therefore, the sequence of clicks in every session is available.




Config:
In the config file, the first line is an integer 'm' defining the number of attributes used from the log data. And the next m lines are the column indexes of attributes in the input file.

The last line is the length of history which here is fixed for all the attributes.




Code:
The models in both "Topic Detection" and "Recommendation" are the same, and only the evaluation part is different.
In Topic Detection, the topics for every attribute are chosen according to a threshold and saved into separate files.

In Recommendation, the rank of items from the test data is computed from obtained Q-values.

```
@inproceedings{tavakol2014factored,
  title={Factored MDPs for detecting topics of user sessions},
  author={Tavakol, Maryam and Brefeld, Ulf},
  booktitle={Proceedings of the 8th ACM Conference on Recommender Systems},
  pages={33--40},
  year={2014},
  organization={ACM}
}
```
