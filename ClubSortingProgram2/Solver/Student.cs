using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class Student
    {
        public readonly string Name;
        public Club[] Assignments { get; private set; }

        public Student(string name)
        {
            Name = name;
            Assignments = new Club[Solver.Settings.Weeks];
        }
    }
}
