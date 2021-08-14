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
        public Club[] Assignment { get; private set; }

        public Student(string name)
        {
            Name = name;
            Assignment = new Club[Solver.Settings.Weeks];
        }
    }
}
