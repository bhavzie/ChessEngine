



/* Making a move 
 
 * 1. make(move) called
 * 2. Get from,to,cap from move
 * 3. Store the current pos in pos->history array
 * 4. Move current piece from -> to
 * 5. if a capture was made,remove captured from piece list
 * 6. Update 50 move rule,see if pawn moved
 * 7. Promotions
 * 8. EnPas Captures
 * 9. Set Enpas squares if PStart ( pawn moves 2 steps forward at the start )
 * 10 For all pieces added,moved,removed update all params from defs.h
 * 11 Position counters,piece lists
 * 12 Maintain poskey
 * 13 Don't forget castle perms
 * 14 Change side,increment ply and hisply

*/

