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
            catch (ArgumentException)
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
            catch (ArgumentException)
            {
                MainScreen.Instance.AlertError("Found duplicate club name: \"" + c.Name + "\".");
            }
        }

        public void Solve()
        {
            if (_solved) return;
            //TODO

            _solved = true;
        }

        public Club[] GetClubsFor(string name)
        {
            if (! _solved)
            {
                MainScreen.Instance.AlertError("Club assignments have not been created yet.");
                return null;
            }
            if (! _students.ContainsKey(name))
            {
                MainScreen.Instance.AlertError("Student \"" + name + "\" does not exist.");
                return null;
            }
            return _students[name].Assignments;
        }

        public Student[] GetStudentsIn(string name)
        {
            if (!_solved)
            {
                MainScreen.Instance.AlertError("Club assignments have not been created yet.");
                return null;
            }
            if (!_clubs.ContainsKey(name))
            {
                MainScreen.Instance.AlertError("Club \"" + name + "\" does not exist.");
                return null;
            }
            return _clubs[name].Assignments.ToArray();
        }

        public struct SolverSettings
        {
            public readonly int MaximumCapacity;
            public readonly int Weeks;
            public readonly string DefaultClubName;
            public readonly bool RequestsHaveOrder;
            public readonly int MaxRequests;

            public SolverSettings(int mc, int w, string dcn, bool rho, int mr)
            {
                MaximumCapacity = mc;
                Weeks = w;
                DefaultClubName = dcn;
                RequestsHaveOrder = rho;
                MaxRequests = mr;
            }
        }
    }
}
