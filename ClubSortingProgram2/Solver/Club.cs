using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class Club
    {
        public readonly string Name;
        private readonly int MaximumCapacity;

        public Club(string name, bool defaultClub = false)
        {
            Name = name;
            MaximumCapacity = defaultClub ? int.MaxValue : Solver.Settings.MaximumCapacity;
        }
    }
}
