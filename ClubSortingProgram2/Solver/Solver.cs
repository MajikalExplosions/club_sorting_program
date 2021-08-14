using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ClubSortingProgram2.Solver
{
    public class Solver
    {
        public static readonly SolverSettings Settings;
        private readonly SortedList<string, Club> _clubs;
        private readonly SortedList<string, Student> _students;

        private bool _solved;

        public Solver()
        {
            _clubs = new SortedList<string, Club>();
            _students = new SortedList<string, Student>();

            _clubs.Add(Settings.DefaultClubName, new Club(Settings.DefaultClubName, true));
        }

        public void AddStudent(Student s)
        {
            try
            {
                _students.Add(s.Name, s);
            }
            catch (ArgumentException e)
            {
                MainScreen.Instance.AlertError("Found duplicate student name: \"" + s.Name + "\".");
            }
        }

        public void AddClub(Club c)
        {
            try
            {
                _clubs.Add(c.Name, c);
            }
            catch (ArgumentException e)
            {
                MainScreen.Instance.AlertError("Found duplicate club name: \"" + c.Name + "\".");
            }
        }

        public void Solve()
        {
            if (_solved) return;
            //TODO
        }

        public Club[] GetStudentAssignments(string name)
        {
            //TODO
            return null;
        }

        public struct SolverSettings
        {
            public readonly int MaximumCapacity;
            public readonly int Weeks;
            public readonly string DefaultClubName;

            public SolverSettings(int mc, int w, string dcn)
            {
                MaximumCapacity = mc;
                Weeks = w;
                DefaultClubName = dcn;
            }
        }
    }
}
