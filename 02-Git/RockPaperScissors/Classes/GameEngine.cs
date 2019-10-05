namespace RockPaperScissors.Classes
{
    public class GameEngine
    {
        private GameValidator validator = new GameValidator();

        public string rps_game_winner(string[][] values)
        {
            var Winner = rps_game_winner_player_values(values);
            return Winner.Name;

        }

        public Player rps_game_winner_player_values(string[][] values)
        {
            validator.CheckPlayerCount(values);

            var firstPlayer = new Player(values[0]);
            var secondPlayer = new Player(values[1]);

            return rps_game_winner_player(firstPlayer, secondPlayer);
        }

        public Player rps_game_winner_player(Player firstPlayer, Player secondPlayer)
        {
            validator.CheckStrategy(firstPlayer.Strategy);
            validator.CheckStrategy(secondPlayer.Strategy);
            return play_game(firstPlayer, secondPlayer);

        }

        /// <summary>
        /// PlayGame
        /// The rules are: R beats S; S beats P; and P beats R
        /// If both players play the same move, the first player is the winner.
        /// </summary>
        public Player play_game(Player firstPlayer, Player secondPlayer)
        {
            var firstPlayerWins =
                (firstPlayer.Strategy == "R" && secondPlayer.Strategy == "S") ||
                (firstPlayer.Strategy == "S" && secondPlayer.Strategy == "P") ||
                (firstPlayer.Strategy == "P" && secondPlayer.Strategy == "R") ||
                (firstPlayer.Strategy == secondPlayer.Strategy);

            return firstPlayerWins ? firstPlayer : secondPlayer;
        }
    }
}
