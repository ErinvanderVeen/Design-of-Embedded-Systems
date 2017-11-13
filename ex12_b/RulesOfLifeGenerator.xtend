package gameOfLife.generator

import gameOfLife.rulesDSL.Game
import gameOfLife.rulesDSL.Operator
import gameOfLife.rulesDSL.State

class RulesOfLifeGenerator {
	
	def static toJava(Game root)'''

		package GameOfLife;
		
		import java.awt.Point;
		import java.util.ArrayList;
		
		public class RulesOfLife {
			public static void computeInitialConfig(ArrayList<Point> survivingCells) {
				survivingCells.add(new Point(22, 22));
				survivingCells.add(new Point(22, 23));
				survivingCells.add(new Point(22, 24));
				survivingCells.add(new Point(21, 23));
				survivingCells.add(new Point(23, 23));
			}
			
			public static void computeSurvivors(boolean[][] gameBoard, ArrayList<Point> nextLiveCells) {
		        // Iterate through the array, follow game of life rules
				for (int i = 1; i < gameBoard.length - 1; i++) {
					for (int j = 1; j < gameBoard[0].length - 1; j++) {
						int surrounding = 0;
		                if (gameBoard[i-1][j-1]) { surrounding++; }
		                if (gameBoard[i-1][j])   { surrounding++; }
		                if (gameBoard[i-1][j+1]) { surrounding++; }
		                if (gameBoard[i][j-1])   { surrounding++; }
		                if (gameBoard[i][j+1])   { surrounding++; }
		                if (gameBoard[i+1][j-1]) { surrounding++; }
		                if (gameBoard[i+1][j])   { surrounding++; }
		                if (gameBoard[i+1][j+1]) { surrounding++; }
						
						
						«FOR rule: root.rules»
						
						if(«toInitState(rule.initstate)» && surrounding «toOperator(rule.operator)» «rule.neighbours»)
						{
							«toEndState(rule.endstate)»	
						}
						«ENDFOR»
		            }
		        }
			}
		
		}
	'''
	
	def static toInitState(State state){
		switch(state){
			case State::ALIVE: 
			return '''gameBoard[i][j]'''
			case State::DEAD:
			return '''!gameBoard[i][j]'''
		}
	}
	def static toEndState(State state){
		switch(state){
			case State::ALIVE: 
			return '''nextLiveCells.add(new Point(i - 1, j - 1));'''
			case State::DEAD:
			return '''nextLiveCells.remove(new Point(i - 1, j - 1));'''
		}
	}
	
	def static toOperator(Operator operator) {
		switch(operator) {
			
			case Operator::EXACTLY:
			return'''=='''
			
			case Operator::FEWER: 
			return '''<'''
			
			case Operator::GREATER: 
			return '''>'''
			
			default: return'''=='''
		}
	}
	
}