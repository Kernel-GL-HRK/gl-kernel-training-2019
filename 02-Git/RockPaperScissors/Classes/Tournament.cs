using System.Collections.Generic;
using System.Linq;

namespace RockPaperScissors.Classes
{
    public class TournamentEngine
    {
        private GameEngine engine = new GameEngine();

        public string[] rps_tournament_winner(string[][][][] values)
        {
            var games = new List<GameEngine>();
            var winners = new List<Player>();
            Player tournamentWinner = null;

            foreach (string[][][] groups in values)
            {
                winners.Add(run_groups_battles(groups));
            }

            tournamentWinner = run_winners_battles(winners);
            return new string[] { tournamentWinner.Name, tournamentWinner.Strategy };
        }

        private Player run_winners_battles(List<Player> winners)
        {
            Player firstPlayer = null;
            Player secondPlayer = null;
            Player winner = null;

            while (winners.Count > 1)
            {
                firstPlayer = winners[0];
                secondPlayer = winners[1];
                winner = engine.rps_game_winner_player(firstPlayer, secondPlayer);

                if (winner == firstPlayer)
                    winners.Remove(secondPlayer);
                else
                    winners.Remove(firstPlayer);
            }

            return winners.First();
        }

        private Player run_groups_battles(string[][][] groups)
        {
            var winners = new List<Player>();
            foreach (string[][] game in groups)
            {
                winners.Add(engine.rps_game_winner_player_values(game));
            }

            return run_winners_battles(winners);
        }

    }
}
