/** @page goboardko Handling of Ko Moves in Class GoBoard

@section goboardkodefault Default Settings

By default moves violating the Ko rule are illegal.
That means that GoBoard::IsLegal will return false.
If they are played anyway, GoBoard::LastMoveInfo will return true
for ::isIllegal and ::isRepetition.

@section goboardkoallow Allowing Ko Moves

During searches, Ko moves can be made legal with GoBoard::AllowKoRepetition.
In addition, a ko loser has to be set with GoBoard::SetKoLoser.
Then, Ko moves are legal (the ::isRepetition flag is still set).
GoBoard::KoLevel returns the number of (now legal) Ko moves played by the
Ko winner so far.
If GoBoard::KoModifiesHash is true, only up to GoBoard::MAX_KOLEVEL Ko
moves will be legal.

@section goboardkonotes Oddities in the Current Implementation

- AllowAnyRepetition overrides AllowKoRepetition.

- GoBoard::KoModifiesHash has an effect only if AllowKoRepetition.

- GoBoard::KoLevel is updated both if AllowKoRepetition and AllowAnyRepetition
(does some code rely on this?)

- GoBoard::KoColor is updated only if AllowKoRepetition
(does some code rely on this?)

- AllowKoRepetition does not allow two non-alternating Ko moves by the same
player in a row
(does some code rely on this?)
*/

