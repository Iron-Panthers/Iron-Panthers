public class Tasks{
	private ArrayList<Task> tasks = new ArrayList<Task>();

	public String addTask(Task newTask){
		//check if the newTask is a dupe
		for (Task task in tasks){
			if (task.name.equals(newTask.name)){
				return "This is a duplicate."
			}
		}

		tasks.add(newTask);
		return "Your task named \""+newTask.name+"\" was successfully added."
	}

	public Task[] tasksForToday(){
		Task[] tasksToday = new Task[tasks.length()]
		int lastAdded = 0;
		for (Task task in tasksToday){
			if (task.daysUntilDue <= 1){
				tasksToday[i] = task;
				i++;
			}
		}
		return tasksToday;
	}

	public boolean allNextDay(){
		for (Task task in tasks){
			task.nextDay();
		}
	}

	public String finishTask(int taskToEnd){
		if (taskToEnd < tasks.length() && tasksToEnd >= 0){
			Task theTask = tasks.get(taskToEnd);
			if (theTask.daysUntilDue > 1){
				return "You finished this task "+(theTask.daysUntilDue-1)+" days early";
			} else if (theTask.daysUntilDue == 1){
				return "You finished this task on time.";
			} else {
				return "You finished this task "+(-1 * theTask.daysUntilDue +1)+" days late.";
			}
			tasks.remove(taskToEnd);
		} else {
			return "Index " + taskToEnd + " out of bounds.";
		}
	}
	
	public Task[] allTasks(){
		Task[] importanceOrderedTasks = new Task[tasks.length()];
		Task[] fullOrderedTasks = new Task[tasks.length()];
		int minDate = 100;
		int maxDate = -100;
		for (Task task in tasks){
			if (task.daysUntilDue < minDate){
				minDate = task.daysUntilDue;
			}
			if (task.daysUntildue > maxDate){
				maxDate = task.daysUntilDue;
			}
		}
		
		maxDate++;
		
		boolean didSwap = true;
		while (didSwap){
			didSwap = false;
			for (int i = tasks.length()-2; i > -1; i--){
				if (tasks.get(i).priority > tasks.get(i+1).priority){
					Task thisTask = tasks.get(i);
					tasks.set(i, tasks.get(i+1));
					tasks.set(i+1, thisTask);
					didSwap = true
				}
			}
		}
		
		int i = 0;
		for (Task task in tasks){
			importanceOrderedTasks[i] = task;
			i++;
		}
		
		for (int curDate = minDate; curDate < maxDate; curDate++){
			for (int i = 0; i < importanceOrderedTasks.length; i++){
				if (importanceOrderedTasks[i].daysUntilDue == curDate){
					fullOrderedTasks[i] = importanceOrderedTasks[i];
				}
			}
		}
	}
}
