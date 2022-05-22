# Arithmetic coder and decoder
This is student project on the subject **Algorithms for encoding and compressing information**.

Arithmetic coding (AC) is a form of entropy encoding used in lossless data compression.

In arithmetic coding, a message is represented by an interval of real numbers between 0 and 1. As the message becomes longer, the interval needed to represent it becomes smaller, and the number of bits needed to specify that interval grows. Successive symbols of the message reduce the size of the interval in accordance with the symbol probabilities generated by the model. The more likely symbols reduce the range by less than the unlikely symbols and hence add fewer bits to the message.

![The probabilities of each character appearing in the text (in descending order) and the ranges corresponding to these characters..](img/CorovaTable1.PNG)


![Graphical process of encoding the first three characters.](img/CorovaPicture.PNG)

Suppose all the decoder knows about the message is the final range. It can immediately deduce  the first character, since the range lies entirely within the space the model of Table allocates for character.
