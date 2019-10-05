using System;
using System.Linq;

namespace RockPaperScissors.Classes
{
    public class GameValidator
    {

        public void CheckPlayerCount(string[][] values)
        {
            if (values.Length != 2)
            {
                throw new WrongNumberOfPlayersError("The number of players is not equal to 2!");
            }
        }

        public void CheckStrategy(string strategy)
        {
            string[] rps = new string[3] { "R", "P", "S" };
            var strategyIsValid = (strategy != null && (strategy.Length == 1 && rps.Contains(strategy)));

            if (!strategyIsValid)
            {
                throw new NoSuchStrategyError(
                    String.Format("Strategy {0} is not valid!", strategy));
            }
        }

    }

}
