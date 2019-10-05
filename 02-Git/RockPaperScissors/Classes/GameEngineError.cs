using System;
using System.Collections.Generic;
using System.Text;

namespace RockPaperScissors.Classes
{
    public abstract class RockPaperScissorsError : Exception
    {
        public RockPaperScissorsError()
        { }

        public RockPaperScissorsError(string message) : base(message)
        { }

        public RockPaperScissorsError(string message, Exception innerException) : base(message, innerException)
        { }

    }

    [Serializable]
    public class WrongNumberOfPlayersError : RockPaperScissorsError
    {
        public WrongNumberOfPlayersError()
        { }

        public WrongNumberOfPlayersError(string message) : base(message)
        { }

        public WrongNumberOfPlayersError(string message, Exception innerException) : base(message, innerException)
        { }
    }

    [Serializable]
    public class NoSuchStrategyError : RockPaperScissorsError
    {
        public NoSuchStrategyError()
        { }

        public NoSuchStrategyError(string message) : base(message)
        { }

        public NoSuchStrategyError(string message, Exception innerException) : base(message, innerException)
        { }
    }
}
